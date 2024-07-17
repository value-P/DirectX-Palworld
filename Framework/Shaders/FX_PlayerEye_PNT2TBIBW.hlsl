#include "Shader_Defines.hlsli"

struct FMeshInfo_PlayerEye
{
    Matrix matBones[256];

    Matrix matParentWorld;

    bool iCorneaIndex;
    float3 vCorneaAlternativeColor;

    bool iHighlightIndex;
    float3 vHighlightAlternativeColor;

    bool iPupilIndex;
    float3 vPupliAlternativeColor;

    bool bCorneaUseAlternativeColor;
    bool bHighlightUseAlternativeColor;
    bool bPupilUseAlternativeColor;
    bool bPad1;

    float fCorneaEmission;
    float fHighlightEmission;
    float fPupliEmission;
    float fMeshPlayerEyePad1;
};

FMeshInfo_PlayerEye g_tMeshInfo;
Texture2D g_Textures[14];

static const int g_iCornea_0 = 0;
static const int g_iCornea_1 = 1;
static const int g_iCornea_2 = 2;
static const int g_iCornea_3 = 3;
static const int g_iHighlight_0 = 4;
static const int g_iHighlight_1 = 5;
static const int g_iHighlight_2 = 6;
static const int g_iHighlight_3 = 7;
static const int g_iHighlight_4 = 8;
static const int g_iHighlight_5 = 9;
static const int g_iPupil_0 = 10;
static const int g_iPupil_1 = 11;
static const int g_iPupil_2 = 12;
static const int g_iPupil_3 = 13;

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

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vDiffuse = vector(1, 1, 1, 1);
    vector vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0);
    vector vMRSO = vector(0, 0.04f, 0, 1);
    vector vEmission = vector(0, 0, 0, 0);
    
    if (g_tMeshInfo.iHighlightIndex == 0)
    {
        vDiffuse = g_Textures[g_iHighlight_0].Sample(g_LinearSampler, In.vTexcoord);
    }
    else if (g_tMeshInfo.iHighlightIndex == 1)
    {
        vDiffuse = g_Textures[g_iHighlight_1].Sample(g_LinearSampler, In.vTexcoord);
    }
    else if (g_tMeshInfo.iHighlightIndex == 2)
    {
        vDiffuse = g_Textures[g_iHighlight_2].Sample(g_LinearSampler, In.vTexcoord);
    }
    else if (g_tMeshInfo.iHighlightIndex == 3)
    {
        vDiffuse = g_Textures[g_iHighlight_3].Sample(g_LinearSampler, In.vTexcoord);
    }
    else if (g_tMeshInfo.iHighlightIndex == 4)
    {
        vDiffuse = g_Textures[g_iHighlight_4].Sample(g_LinearSampler, In.vTexcoord);
    }
    else if (g_tMeshInfo.iHighlightIndex == 5)
    {
        vDiffuse = g_Textures[g_iHighlight_5].Sample(g_LinearSampler, In.vTexcoord);
    }
    
    if(g_tMeshInfo.bHighlightUseAlternativeColor)
    {
        vDiffuse = float4(mul(vDiffuse.x + vDiffuse.y + vDiffuse.z, g_tMeshInfo.vCorneaAlternativeColor), vDiffuse.a);
    }
       
    if (vDiffuse.a == 0)
    {
        if (g_tMeshInfo.iPupilIndex == 0)
        {
            vDiffuse = g_Textures[g_iPupil_0].Sample(g_LinearSampler, In.vTexcoord);
        }
        else if (g_tMeshInfo.iPupilIndex == 1)
        {
            vDiffuse = g_Textures[g_iPupil_1].Sample(g_LinearSampler, In.vTexcoord);
        }
        else if (g_tMeshInfo.iPupilIndex == 2)
        {
            vDiffuse = g_Textures[g_iPupil_2].Sample(g_LinearSampler, In.vTexcoord);
        }
        else if (g_tMeshInfo.iPupilIndex == 3)
        {
            vDiffuse = g_Textures[g_iPupil_3].Sample(g_LinearSampler, In.vTexcoord);
        }
       
        if (g_tMeshInfo.bPupilUseAlternativeColor)
        {
            vDiffuse = float4(mul(vDiffuse.x + vDiffuse.y + vDiffuse.z, g_tMeshInfo.vPupliAlternativeColor), vDiffuse.a);
        }
    }
        
    if (vDiffuse.a == 0)
    {
        if (g_tMeshInfo.iCorneaIndex == 0)
        {
            vDiffuse = g_Textures[g_iCornea_0].Sample(g_LinearSampler, In.vTexcoord);
        }
        else if (g_tMeshInfo.iCorneaIndex == 1)
        {
            vDiffuse = g_Textures[g_iCornea_1].Sample(g_LinearSampler, In.vTexcoord);
        }
        else if (g_tMeshInfo.iCorneaIndex == 2)
        {
            vDiffuse = g_Textures[g_iCornea_2].Sample(g_LinearSampler, In.vTexcoord);
        }
        else if (g_tMeshInfo.iCorneaIndex == 3)
        {
            vDiffuse = g_Textures[g_iCornea_3].Sample(g_LinearSampler, In.vTexcoord);
        }
        
        if (g_tMeshInfo.bCorneaUseAlternativeColor)
        {
            vDiffuse = float4(mul(vDiffuse.x + vDiffuse.y + vDiffuse.z, g_tMeshInfo.vCorneaAlternativeColor), vDiffuse.a);
        }
    }
   
    if (vDiffuse.a == 0)
    {
        vDiffuse = float4(1, 1, 1, 1);
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
    
    vector vDiffuse = vector(1, 1, 1, 1);
    
    if (g_tMeshInfo.iHighlightIndex == 0)
    {
        vDiffuse = g_Textures[g_iHighlight_0].Sample(g_LinearSampler, In.vTexcoord);
    }
    else if (g_tMeshInfo.iHighlightIndex == 1)
    {
        vDiffuse = g_Textures[g_iHighlight_1].Sample(g_LinearSampler, In.vTexcoord);
    }
    else if (g_tMeshInfo.iHighlightIndex == 2)
    {
        vDiffuse = g_Textures[g_iHighlight_2].Sample(g_LinearSampler, In.vTexcoord);
    }
    else if (g_tMeshInfo.iHighlightIndex == 3)
    {
        vDiffuse = g_Textures[g_iHighlight_3].Sample(g_LinearSampler, In.vTexcoord);
    }
    else if (g_tMeshInfo.iHighlightIndex == 4)
    {
        vDiffuse = g_Textures[g_iHighlight_4].Sample(g_LinearSampler, In.vTexcoord);
    }
    else if (g_tMeshInfo.iHighlightIndex == 5)
    {
        vDiffuse = g_Textures[g_iHighlight_5].Sample(g_LinearSampler, In.vTexcoord);
    }
    
    if (g_tMeshInfo.bHighlightUseAlternativeColor)
    {
        vDiffuse = float4(mul(vDiffuse.x + vDiffuse.y + vDiffuse.z, g_tMeshInfo.vCorneaAlternativeColor), vDiffuse.a);
    }
       
    if (vDiffuse.a == 0)
    {
        if (g_tMeshInfo.iPupilIndex == 0)
        {
            vDiffuse = g_Textures[g_iPupil_0].Sample(g_LinearSampler, In.vTexcoord);
        }
        else if (g_tMeshInfo.iPupilIndex == 1)
        {
            vDiffuse = g_Textures[g_iPupil_1].Sample(g_LinearSampler, In.vTexcoord);
        }
        else if (g_tMeshInfo.iPupilIndex == 2)
        {
            vDiffuse = g_Textures[g_iPupil_2].Sample(g_LinearSampler, In.vTexcoord);
        }
        else if (g_tMeshInfo.iPupilIndex == 3)
        {
            vDiffuse = g_Textures[g_iPupil_3].Sample(g_LinearSampler, In.vTexcoord);
        }
       
        if (g_tMeshInfo.bPupilUseAlternativeColor)
        {
            vDiffuse = float4(mul(vDiffuse.x + vDiffuse.y + vDiffuse.z, g_tMeshInfo.vPupliAlternativeColor), vDiffuse.a);
        }
    }
        
    if (vDiffuse.a == 0)
    {
        if (g_tMeshInfo.iCorneaIndex == 0)
        {
            vDiffuse = g_Textures[g_iCornea_0].Sample(g_LinearSampler, In.vTexcoord);
        }
        else if (g_tMeshInfo.iCorneaIndex == 1)
        {
            vDiffuse = g_Textures[g_iCornea_1].Sample(g_LinearSampler, In.vTexcoord);
        }
        else if (g_tMeshInfo.iCorneaIndex == 2)
        {
            vDiffuse = g_Textures[g_iCornea_2].Sample(g_LinearSampler, In.vTexcoord);
        }
        else if (g_tMeshInfo.iCorneaIndex == 3)
        {
            vDiffuse = g_Textures[g_iCornea_3].Sample(g_LinearSampler, In.vTexcoord);
        }
        
        if (g_tMeshInfo.bCorneaUseAlternativeColor)
        {
            vDiffuse = float4(mul(vDiffuse.x + vDiffuse.y + vDiffuse.z, g_tMeshInfo.vCorneaAlternativeColor), vDiffuse.a);
        }
    }
   
    if (vDiffuse.a == 0)
    {
        vDiffuse = float4(1, 1, 1, 1);
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