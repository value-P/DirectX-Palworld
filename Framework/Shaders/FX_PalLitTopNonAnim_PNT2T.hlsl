#include "Shader_Defines.hlsli"

struct FMeshInfo_PalLitTop_NonAnim
{
    Matrix matTransform;

    Matrix matWorld;

    bool bDiffuse;
    bool bNormal;
    bool bMRSO;
    bool bEmission;

    bool bTopDiffuse;
    bool bTopNormal;
    float fRoughnessAdd;
    float fOcclusionAdd;
};

FMeshInfo_PalLitTop_NonAnim g_tMeshInfo;
Texture2D g_Textures[6];

static const int g_iDiffuse = 0;
static const int g_iNormal = 1;
static const int g_iMRSO = 2;
static const int g_iEmission = 3;
static const int g_iTopDiffuse = 4;
static const int g_iTopNormal = 5;

struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float3 vTangent : TANGENT;
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
    
    Matrix matTransform = transpose(g_tMeshInfo.matTransform);
    
    // 로컬 변환 행렬 적용
    float4 vLocalPosition = mul(float4(In.vPosition, 1), matTransform);
    float4 vLocalNormal = mul(float4(In.vNormal, 0), matTransform);
    float4 vLocalTangent = mul(float4(In.vTangent, 0), matTransform);
    
    // 월드 행렬 ID 값 갱신
    matrix matWorld = transpose(g_tMeshInfo.matWorld);
    int iGameObjectID = matWorld._44;
    matWorld._44 = 1;
   
    // 월드 뷰 투영 행렬
    matrix matWVP = mul(mul(matWorld, g_tCamera.matView), g_tCamera.matProj);
    
    Out.vPosition = mul(vLocalPosition, matWVP);
    Out.vNormal = normalize(mul(vLocalNormal, matWorld));
    Out.vTexcoord = In.vTexcoord;
    Out.vWorldPosition = mul(vLocalPosition, matWorld);
    Out.vWorldPosition.w = iGameObjectID;
    Out.vProjPosition = Out.vPosition;
    Out.vTangent = normalize(mul(vLocalTangent, matWorld));
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

    vector vDiffuse = g_tFrame.iDebugRenderTarget == 2 ? vector(1, 0, 1, 1) : vector(0, 0, 0, 0);
    vector vNormal = In.vNormal;
    vector vMRSO = vector(0, 0.04f, 0, 0);
    vector vEmission = vector(0, 0, 0, 0);
    
    // 법선과 수직의 각도 계산
    float fDegree = degrees(dot(float3(0, 1, 0), normalize(vNormal.xyz)));
        
    // 법선이 수직과 70도 이하 차이가 날 떄 ( 라디안 )
    if (fDegree <= 70.f)
    {
        // 비율 구하기
        // 수직과 완전히 일치할 때 DefaultRatio는 0이 된다.
        float fDefaultRatio = saturate((fDegree + 0.000001f) / 70.f);
        float4 vDefaultDiffuse = float4(0, 0, 0, 0);
        float4 vTopDiffuse = float4(0, 0, 0, 0);
        
        // 기본 디퓨즈
        if (g_tMeshInfo.bDiffuse)
        {
            vDefaultDiffuse = g_Textures[g_iDiffuse].Sample(g_LinearSampler, In.vTexcoord);
        }
        
        // Top 디퓨즈
        if (g_tMeshInfo.bTopDiffuse)
        {
            vTopDiffuse = g_Textures[g_iTopDiffuse].Sample(g_LinearSampler, In.vTexcoord);
        }
        
        if (g_tMeshInfo.bDiffuse && g_tMeshInfo.bTopDiffuse)
        {
            // 디퓨즈 혼합
            vDiffuse = (vDefaultDiffuse * fDefaultRatio) + (vTopDiffuse * (1 - fDefaultRatio));
        }
        else if (g_tMeshInfo.bDiffuse)
        {
            vDiffuse = vDefaultDiffuse;
        }
        else if (g_tMeshInfo.bTopDiffuse)
        {
            vDiffuse = vTopDiffuse;
        }
     
        // 노말 맵핑
        float4 vGeoNormal = float4(In.vNormal.xyz * 0.5f + 0.5f, 0);
        float4 vDefaultNormal = float4(0, 0, 0, 0);
        float4 vTopNormal = float4(0, 0, 0, 0);
        float3x3 matNormalWorld = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);
        
        if (g_tMeshInfo.bNormal)
        {
            vDefaultNormal = g_Textures[g_iNormal].Sample(g_LinearSampler, In.vTexcoord);
            vDefaultNormal = normalize(vNormal * 2 - 1);
            vDefaultNormal = vector(mul(vNormal.xyz, matNormalWorld), 0);
            vDefaultNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
        }
        
        if (g_tMeshInfo.bTopNormal)
        {
            vTopNormal = g_Textures[g_iTopNormal].Sample(g_LinearSampler, In.vTexcoord);
            vTopNormal = normalize(vNormal * 2 - 1);
            vTopNormal = vector(mul(vNormal.xyz, matNormalWorld), 0);
            vTopNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
        }
        
        // 두 노말 텍스처가 모두 존재할 때
        // 노말 혼합
        if (g_tMeshInfo.bNormal && g_tMeshInfo.bTopNormal)
        {
            // Half Normal 구하기
            vNormal = normalize(lerp(vTopNormal, vDefaultNormal, fDefaultRatio));
        }
        else if (g_tMeshInfo.bNormal)
        {
            vNormal = vDefaultNormal;
        }
        else if (g_tMeshInfo.bTopNormal)
        {
            vNormal = vTopNormal;
        }
    }
    else
    {
        if (g_tMeshInfo.bDiffuse)
        {
            vDiffuse = g_Textures[g_iDiffuse].Sample(g_LinearSampler, In.vTexcoord);
        }
        
        if (g_tMeshInfo.bNormal)
        {
            float3x3 matNormalWorld = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);
            vector vNormalFromTex = g_Textures[g_iNormal].Sample(g_LinearSampler, In.vTexcoord);
            vNormal = vector(vNormalFromTex.xyz * 2.f - 1.f, 0);
            vNormal = vector(mul(vNormal.xyz, matNormalWorld), 0);
            vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
        }
        else
        {
            vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0);
        }
    }
    
    if (vDiffuse.a <= 0.1f)
    {
        discard;
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
}