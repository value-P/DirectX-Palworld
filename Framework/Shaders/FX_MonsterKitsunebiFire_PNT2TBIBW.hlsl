#include "Shader_Defines.hlsli"

struct FMeshInfo_Default_Anim
{
    Matrix matBones[256];
    Matrix matParentWorld;

    bool bDiffuse;
    bool bNormal;
    bool bMRSO;
    bool bEmission;

    bool bUseAlternativeColor;
    float3 vAlternativeColor;

    float fRoughnessAdd;
    float fOcclusionAdd;
    float2 vPad1;
    
    float fDeltaSeconds;
};

FMeshInfo_Default_Anim g_tMeshInfo;
Texture2D g_Textures[4];

static const int g_iDiffuse = 0;
static const int g_iNormal = 1;
static const int g_iMRSO = 2;
static const int g_iEmission = 3;

struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float3 vTangent : TANGENT;
    uint4 vBlendIndices : BLENDINDEX;
    float4 vBlendWeights : BLENDWEIGHT;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPosition : POSITION0;
    float4 vProjPosition : POSITION1;
    float4 vTangent : TANGENT;
    float4 vBinormal : BINORMAL;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
    
    // 월드 행렬 ID 값 갱신
    matrix matWorld = transpose(g_tMeshInfo.matParentWorld);
    int iGameObjectID = matWorld._44;
    matWorld._44 = 1;
    
    // 월드 뷰 투영 행렬
    matrix matWVP = mul(mul(matWorld, g_tCamera.matView), g_tCamera.matProj);
    
    float fWeightW = 1.f - (In.vBlendWeights.x + In.vBlendWeights.y + In.vBlendWeights.z);
    matrix matBone = mul(transpose(g_tMeshInfo.matBones[In.vBlendIndices.x]), In.vBlendWeights.x) +
                     mul(transpose(g_tMeshInfo.matBones[In.vBlendIndices.y]), In.vBlendWeights.y) +
                     mul(transpose(g_tMeshInfo.matBones[In.vBlendIndices.z]), In.vBlendWeights.z) +
                     mul(transpose(g_tMeshInfo.matBones[In.vBlendIndices.w]), fWeightW);
	
    vector vBoneAppliedPosition = mul(float4(In.vPosition, 1), matBone);
    vector vBoneAppliedNormal = mul(float4(In.vNormal, 0), matBone);
    vector vBoneAppliedTangent = mul(float4(In.vTangent, 0), matBone);

    Out.vPosition = mul(vBoneAppliedPosition, matWVP);
    Out.vNormal = normalize(mul(vBoneAppliedNormal, matWorld));
    Out.vTexcoord = In.vTexcoord;
    Out.vWorldPosition = mul(vBoneAppliedPosition, matWorld);
    Out.vWorldPosition.w = iGameObjectID;
    Out.vProjPosition = Out.vPosition;
    Out.vTangent = normalize(mul(vBoneAppliedTangent, matWorld));
    Out.vBinormal = normalize(vector(cross(Out.vNormal.xyz, Out.vTangent.xyz), 0.f));

    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPosition : POSITION0;
    float4 vProjPosition : POSITION1;
    float4 vTangent : TANGENT;
    float4 vBinormal : BINORMAL;
};

struct PS_OUT
{
    float4 vDiffuse : SV_TARGET0;
    float4 vNormal : SV_TARGET1;
    float4 vMRSO : SV_TARGET2;
    float4 vEmission : SV_TARGET3;
    float fDepth : SV_TARGET4;
    float4 vWorldPosition : SV_TARGET5;
};

float Firenoise(float3 p) //Thx to Las^Mercury
{
    float3 i = floor(p);
    float4 a = dot(i, float3(1., 57., 21.)) + float4(0., 57., 21., 78.);
    float3 f = cos((p - i) * acos(-1.0)) * (-0.5) + 0.5;
    a = lerp(sin(cos(a) * a), sin(cos(1.0 + a) * (1.0 + a)), f.x);
    a.xy = lerp(a.xz, a.yw, f.y);
    return lerp(a.x, a.y, f.z);
}

float sphere(float3 p, float4 spr)
{
    return length(spr.xyz - p) - spr.w;
}

float flame(float3 p)
{
    float d = sphere(p * float3(0.75, 0.6, 1.), float4(0.0, -1.0, 0.0, 1.0));
    return d + (Firenoise(p + float3(0.0, g_tMeshInfo.fDeltaSeconds * 8.0, 0.0)) + Firenoise(p * 3.0) * 0.5) * 0.25 * (p.y);
}

float scene(float3 p)
{
    return min(100.0 - length(p), abs(flame(p)));
}

float4 raymarch(float3 org, float3 dir)
{
    float d = 0.0, glow = 0.0, eps = 0.02;
    float3 p = org;
    bool glowed = false;
    
    for (int i = 0; i < 64; i++)
    {
        d = scene(p) + eps;
        p += d * dir;
        if (d > eps)
        {
            if (flame(p) < 0.0)
                glowed = true;
            if (glowed)
                glow = float(i) / 64.0;
        }
    }
    return float4(p, glow);
}

float4 KitsnebiTail(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float2 v = -1.0 + 2.0 * float2(In.vTexcoord.x, 1.0 - In.vTexcoord.y);
    v *= 5.f;
    v.y -= 0.4f;
    
    float3 org = float3(0.0, -2.0, 4.0);
    float3 dir = normalize(float3(v.x * 1.2, -v.y, -1.0));
    
    float4 p = raymarch(org, dir);
    float glow = p.w;
    
    float4 col = lerp(float4(1.0, 0.5, 0.1, 1.0), float4(1.0, 0.5, 0.1, 1.0), p.y * 0.02 + 0.4);
    
    col = lerp(float4(0.0, 0.0, 0.0, 0.0), col, pow(glow * 2.0, 4.0));
    
    if (0.1f > col.a)
        discard;
    
    return col;
}

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vDiffuse = g_tFrame.iDebugRenderTarget == 2 ? KitsnebiTail(In) : vector(0, 0, 0, 0);
    vector vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0);
    vector vMRSO = vector(0, 0.04f, 0, 0);
    vector vEmission = vector(0, 0, 0, 0);
    
    if (g_tMeshInfo.bDiffuse)
    {
        float vNoise = g_Textures[g_iDiffuse].Sample(g_LinearSampler, In.vTexcoord + float2(g_tFrame.fTotalTime / 2.5f, -g_tFrame.fTotalTime / 2.5f)).r;
        float3 vColor1 = float3(0.027875f, 0, 0);
        float3 vColor2 = float3(1.f, 0.043324f, 0);
        float3 vColor3 = float3(1.f, 0.5f, 0);
        float vRatio = (1.f - In.vTexcoord.y);
        
        if (vRatio <= 0.5f)
        {
            vDiffuse = float4(lerp(vColor1, vColor2, (vRatio / 0.5f)), 0);
        }
        else
        {
            vDiffuse = float4(lerp(vColor2, vColor3, ((vRatio - 0.5f) / 0.5f)), 0);
        }

        vDiffuse.a = vDiffuse.r + (1.f - In.vTexcoord.y);
        vEmission = vDiffuse;
    }
    
    if (vDiffuse.a < 0.1f)
    {
        discard;
    }
    
    if (g_tMeshInfo.bNormal)
    {
        float3x3 matNormalWorld = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);
        vector vNormalFromTex = g_Textures[g_iNormal].Sample(g_LinearSampler, In.vTexcoord);
        vNormal = vector(vNormalFromTex.xyz * 2.f - 1.f, 0);
        vNormal = vector(mul(vNormal.xyz, matNormalWorld), 0);
        vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
    }
  
    if (g_tMeshInfo.bMRSO)
    {
        vMRSO = g_Textures[g_iMRSO].Sample(g_LinearSampler, In.vTexcoord);
    }
   
    if (g_tMeshInfo.bEmission)
    {
        vEmission = g_Textures[g_iEmission].Sample(g_LinearSampler, In.vTexcoord);
    }
    
    Out.vDiffuse = vDiffuse;
    Out.vNormal = normalize(vNormal);
    Out.vMRSO = vMRSO;
    Out.vEmission = vEmission;
    Out.fDepth = (In.vProjPosition.z / In.vProjPosition.w);
    Out.vWorldPosition = In.vWorldPosition;
    return Out;
}

struct VS_BACKBUFFER_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float3 vTangent : TANGENT;
    uint4 vBlendIndices : BLENDINDEX;
    float4 vBlendWeights : BLENDWEIGHT;
};

struct VS_BACKBUFFER_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

VS_BACKBUFFER_OUT VS_MAIN_BACKBUFFER(VS_BACKBUFFER_IN In)
{
    VS_BACKBUFFER_OUT Out = (VS_BACKBUFFER_OUT) 0;
    
    // 월드 행렬 ID 값 갱신
    matrix matWorld = transpose(g_tMeshInfo.matParentWorld);
    int iGameObjectID = matWorld._44;
    matWorld._44 = 1;
    
    // 월드 뷰 투영 행렬
    matrix matWVP = mul(mul(matWorld, g_tCamera.matView), g_tCamera.matProj);
    
    float fWeightW = 1.f - (In.vBlendWeights.x + In.vBlendWeights.y + In.vBlendWeights.z);
    matrix matBone = mul(transpose(g_tMeshInfo.matBones[In.vBlendIndices.x]), In.vBlendWeights.x) +
                     mul(transpose(g_tMeshInfo.matBones[In.vBlendIndices.y]), In.vBlendWeights.y) +
                     mul(transpose(g_tMeshInfo.matBones[In.vBlendIndices.z]), In.vBlendWeights.z) +
                     mul(transpose(g_tMeshInfo.matBones[In.vBlendIndices.w]), fWeightW);
	
    vector vBoneAppliedPosition = mul(float4(In.vPosition, 1), matBone);

    Out.vPosition = mul(vBoneAppliedPosition, matWVP);
    Out.vTexcoord = In.vTexcoord;

    return Out;
}

struct PS_BACKBUFFER_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct PS_BACKBUFFER_OUT
{
    float4 vBackBuffer : SV_Target0;
    float4 vBackBuffer_Copy : SV_Target1;
};

PS_BACKBUFFER_OUT PS_MAIN_BACKBUFFER(PS_BACKBUFFER_IN In)
{
    PS_BACKBUFFER_OUT Out = (PS_BACKBUFFER_OUT) 0;

    vector vDiffuse = vector(1, 0, 1, 1);
    
    if (g_tMeshInfo.bDiffuse)
    {
        float vNoise = g_Textures[g_iDiffuse].Sample(g_LinearSampler, In.vTexcoord + float2(g_tFrame.fTotalTime / 2.5f, -g_tFrame.fTotalTime / 2.5f)).r;
        float3 vColor1 = float3(0.027875f, 0, 0);
        float3 vColor2 = float3(1.f, 0.043324f, 0);
        float3 vColor3 = float3(1.f, 0.5f, 0);
        float vRatio = (1.f - In.vTexcoord.y);
        
        if (vRatio <= 0.5f)
        {
            vDiffuse = float4(lerp(vColor1, vColor2, (vRatio / 0.5f)), 0);
        }
        else
        {
            vDiffuse = float4(lerp(vColor2, vColor3, ((vRatio - 0.5f) / 0.5f)), 0);
        }

        vDiffuse.a = vDiffuse.r + (1.f - In.vTexcoord.y);
    }
    
    if (vDiffuse.a < 0.1f)
    {
        discard;
    }
    
    if (g_tMeshInfo.bUseAlternativeColor)
    {
        vDiffuse = float4(mul(vDiffuse.x + vDiffuse.y + vDiffuse.z, g_tMeshInfo.vAlternativeColor), vDiffuse.a);
    }
    
    Out.vBackBuffer = vDiffuse;
    Out.vBackBuffer_Copy = vDiffuse;
    
    return Out;
}

technique11 DefaultTechnique
{
    pass Default
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass NonCull
    {
        SetRasterizerState(RS_None_Cull);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass AlphaBlend
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass NonCull_AlphaBlend
    {
        SetRasterizerState(RS_None_Cull);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass ReverseAlphaBlend
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlending_Reverse, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass NonCull_ReverseAlphaBlend
    {
        SetRasterizerState(RS_None_Cull);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlending_Reverse, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass Blend
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Blending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass NonCull_Blend
    {
        SetRasterizerState(RS_None_Cull);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Blending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass Default_BackBuffer
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN_BACKBUFFER();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_BACKBUFFER();
    }

    pass NonCull_BackBuffer
    {
        SetRasterizerState(RS_None_Cull);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN_BACKBUFFER();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_BACKBUFFER();
    }

    pass AlphaBlend_BackBuffer
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN_BACKBUFFER();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_BACKBUFFER();
    }

    pass NonCull_AlphaBlend_BackBuffer
    {
        SetRasterizerState(RS_None_Cull);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN_BACKBUFFER();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_BACKBUFFER();
    }

    pass ReverseAlphaBlend_BackBuffer
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlending_Reverse, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN_BACKBUFFER();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_BACKBUFFER();
    }

    pass NonCull_ReverseAlphaBlend_BackBuffer
    {
        SetRasterizerState(RS_None_Cull);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlending_Reverse, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN_BACKBUFFER();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_BACKBUFFER();
    }

    pass Blend_BackBuffer
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Blending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN_BACKBUFFER();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_BACKBUFFER();
    }

    pass NonCull_Blend_BackBuffer
    {
        SetRasterizerState(RS_None_Cull);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Blending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN_BACKBUFFER();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_BACKBUFFER();
    }
}