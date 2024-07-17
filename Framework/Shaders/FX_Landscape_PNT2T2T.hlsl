#include "Shader_Defines.hlsli"

struct FMeshInfo_Landscape
{
    Matrix matCombindWorld;

    float fVoronoiCellSizeX;
    float fVoronoiCellSizeY;

    float fNumRepeatX;
    float fNumRepeatY;
};

FMeshInfo_Landscape g_tMeshInfo;
Texture2D g_Textures[49];

// Landscape Materials
static const int g_iColor_GrassA = 0;
static const int g_iNormal_GrassA = 1;
static const int g_iGrassA_Color_Variation = 2;
static const int g_iColor_GrassB = 3;
static const int g_iColor_LongGrass = 4;
static const int g_iColor_DirtAndRock = 5;
static const int g_iNormal_DirtAndRock = 6;
static const int g_iColor_Cliff = 7;
static const int g_iMaCliff = 8;
static const int g_iNormal_Cliff = 9;
static const int g_iColor_SandA = 10;
static const int g_iNormal_SandA = 11;
static const int g_iColor_SandB = 12;
static const int g_iNormal_SandB = 13;
static const int g_iColor_SandC = 14;
static const int g_iNormal_SandC = 15;
static const int g_iColor_NaturalRoadPath = 16;
static const int g_iNormal_NaturalRoadPath = 17;
static const int g_iColor_DirtDry = 18;
static const int g_iNormal_DirtDry = 19;
static const int g_iColor_DirtWet = 20;
static const int g_iNormal_DirtWet = 21;
static const int g_iColor_RiverBed = 22;
static const int g_iMaRiverBed = 23;
static const int g_iNormal_RiverBed = 24;
static const int g_iColor_CoastBeach = 25;
static const int g_iMaCoastBeach = 26;
static const int g_iNormal_CoastBeach = 27;
static const int g_iColor_CoastRock = 28;
static const int g_iMaCoastRock = 29;
static const int g_iNormal_CoastRock = 30;
static const int g_iColor_ForestMoss = 31;
static const int g_iNormal_ForestMoss = 32;
static const int g_iColor_ForestDeadBranches = 33;
static const int g_iNormal_ForestDeadBranches = 34;

// LayerInfos
static const int g_iHeightmap = 35;
static const int g_iLayerInfo_SlopeMaterialA = 36;
static const int g_iLayerInfo_Cliff = 37;
static const int g_iLayerInfo_CoastBeach = 38;
static const int g_iLayerInfo_DirtAndRock = 39;
static const int g_iLayerInfo_DirtDry = 40;
static const int g_iLayerInfo_Foliage_Remover = 41;
static const int g_iLayerInfo_GrassA = 42;
static const int g_iLayerInfo_LongGrass = 43;
static const int g_iLayerInfo_NaturalRoadPath = 44;
static const int g_iLayerInfo_RiverBed = 45;
static const int g_iLayerInfo_SandA = 46;
static const int g_iLayerInfo_SandC = 47;

// Voronoi Diagram
static const int g_iVoronoi = 48;

struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float2 vBaseTexcoord : TEXCOORD0;
    float2 vMaskTexcoord : TEXCOORD1;
    float3 vTangent : TANGENT;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vBaseTexcoord : TEXCOORD0;
    float2 vMaskTexcoord : TEXCOORD1;
    float4 vWorldPosition : POSITION0;
    float4 vProjPosition : POSITION1;
    float4 vTangent : TANGENT;
    float4 vBinormal : BINORMAL;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
    
    // 월드 행렬 ID 값 갱신
    matrix matWorld = transpose(g_tMeshInfo.matCombindWorld);
    int iGameObjectID = matWorld._44;
    matWorld._44 = 1;
    
    // 월드 뷰 투영 행렬
    matrix matWVP = mul(mul(matWorld, g_tCamera.matView), g_tCamera.matProj);
    
    Out.vPosition = mul(float4(In.vPosition, 1), matWVP);
    Out.vNormal = normalize(mul(float4(In.vNormal, 0), matWorld));
    Out.vBaseTexcoord = In.vBaseTexcoord;
    Out.vMaskTexcoord = In.vMaskTexcoord;
    Out.vWorldPosition = mul(float4(In.vPosition, 1), matWorld);
    Out.vWorldPosition.w = iGameObjectID;
    Out.vProjPosition = Out.vPosition;
    Out.vTangent = normalize(mul(float4(In.vTangent, 0), matWorld));
    Out.vBinormal = normalize(vector(cross(Out.vNormal.xyz, Out.vTangent.xyz), 0.f));
    
    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vBaseTexcoord : TEXCOORD0;
    float2 vMaskTexcoord : TEXCOORD1;
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
    
    vector vDiffuse = g_tFrame.iDebugRenderTarget == 2 ? vector(1, 0, 1, 0) : vector(0, 0, 0, 0);
    vector vNormal = In.vNormal;
    float4 vRSAO = float4(0.4f, 0, 1, 0);
    float4 vEmission = float4(0, 0, 0, 0);

    // Layer Info
    float vHeightMap = g_Textures[g_iHeightmap].Sample(g_LinearSampler, In.vMaskTexcoord).r;
    float vLayer_SlopeMaterialA = g_Textures[g_iLayerInfo_SlopeMaterialA].Sample(g_LinearSampler, In.vMaskTexcoord).r;
    float vLayer_Cliff = g_Textures[g_iLayerInfo_Cliff].Sample(g_LinearSampler, In.vMaskTexcoord).r;
    float vLayer_CoastBeach = g_Textures[g_iLayerInfo_CoastBeach].Sample(g_LinearSampler, In.vMaskTexcoord).r;
    float vLayer_DirtAndRock = g_Textures[g_iLayerInfo_DirtAndRock].Sample(g_LinearSampler, In.vMaskTexcoord).r;
    float vLayer_DirtDry = g_Textures[g_iLayerInfo_DirtDry].Sample(g_LinearSampler, In.vMaskTexcoord).r;
    float vLayer_Foliage_Remover = g_Textures[g_iLayerInfo_Foliage_Remover].Sample(g_LinearSampler, In.vMaskTexcoord).r;
    float vLayer_GrassA = g_Textures[g_iLayerInfo_GrassA].Sample(g_LinearSampler, In.vMaskTexcoord).r;
    float vLayer_LongGrass = g_Textures[g_iLayerInfo_LongGrass].Sample(g_LinearSampler, In.vMaskTexcoord).r;
    float vLayer_NaturalRoadPath = g_Textures[g_iLayerInfo_NaturalRoadPath].Sample(g_LinearSampler, In.vMaskTexcoord).r;
    float vLayer_RiverBed = g_Textures[g_iLayerInfo_RiverBed].Sample(g_LinearSampler, In.vMaskTexcoord).r;
    float vLayer_SandA = g_Textures[g_iLayerInfo_SandA].Sample(g_LinearSampler, In.vMaskTexcoord).r;
    float vLayer_SandC = g_Textures[g_iLayerInfo_SandC].Sample(g_LinearSampler, In.vMaskTexcoord).r;
    
    
    float2 vVoronoiTexCoord = float2(In.vBaseTexcoord.x / 80, In.vBaseTexcoord.y / 80);
    
    vector vVoronoi = g_Textures[g_iVoronoi].Sample(g_LinearSampler, vVoronoiTexCoord);
    
    float2 vAdded = In.vBaseTexcoord + vVoronoiTexCoord;
    
    float fRadian = radians(vVoronoi.z);
    float fCos = cos(fRadian);
    float fSin = sin(fRadian);
    
    float2 vRotated = float2(fCos * vAdded.x - fSin * vAdded.y, fSin * vAdded.x + fCos * vAdded.y);
    
    
    if (vVoronoi.a <= 0.25f)
    {
        vRotated = In.vBaseTexcoord;
    }
    vRotated *= 0.25f;
    
    // Diffuse
    vDiffuse =
        (vLayer_SlopeMaterialA * g_Textures[g_iColor_GrassA].Sample(g_LinearSampler, vRotated)) +
        (vLayer_Cliff * g_Textures[g_iColor_Cliff].Sample(g_LinearSampler, vRotated)) +
        (vLayer_CoastBeach * g_Textures[g_iColor_CoastBeach].Sample(g_LinearSampler, vRotated)) +
        (vLayer_DirtAndRock * g_Textures[g_iColor_DirtAndRock].Sample(g_LinearSampler, vRotated)) +
        (vLayer_DirtDry * g_Textures[g_iColor_DirtDry].Sample(g_LinearSampler, vRotated)) +
        (vLayer_GrassA * g_Textures[g_iColor_GrassA].Sample(g_LinearSampler, vRotated)) +
        (vLayer_LongGrass * g_Textures[g_iColor_LongGrass].Sample(g_LinearSampler, vRotated)) +
        (vLayer_NaturalRoadPath * g_Textures[g_iColor_NaturalRoadPath].Sample(g_LinearSampler, vRotated)) +
        (vLayer_RiverBed * g_Textures[g_iColor_RiverBed].Sample(g_LinearSampler, vRotated)) +
        (vLayer_SandA * g_Textures[g_iColor_SandA].Sample(g_LinearSampler, vRotated)) +
        (vLayer_SandC * g_Textures[g_iColor_SandC].Sample(g_LinearSampler, vRotated));
    
    vNormal =
        (vLayer_SlopeMaterialA * normalize(g_Textures[g_iNormal_GrassA].Sample(g_LinearSampler, In.vBaseTexcoord))) +
        (vLayer_Cliff * normalize(g_Textures[g_iNormal_Cliff].Sample(g_LinearSampler, In.vBaseTexcoord))) +
        (vLayer_CoastBeach * normalize(g_Textures[g_iNormal_CoastBeach].Sample(g_LinearSampler, In.vBaseTexcoord))) +
        (vLayer_DirtAndRock * normalize(g_Textures[g_iNormal_DirtAndRock].Sample(g_LinearSampler, In.vBaseTexcoord))) +
        (vLayer_DirtDry * normalize(g_Textures[g_iNormal_DirtDry].Sample(g_LinearSampler, In.vBaseTexcoord))) +
        (vLayer_GrassA * normalize(g_Textures[g_iNormal_GrassA].Sample(g_LinearSampler, In.vBaseTexcoord))) +
        (vLayer_LongGrass * normalize(g_Textures[g_iNormal_GrassA].Sample(g_LinearSampler, In.vBaseTexcoord))) +
        (vLayer_NaturalRoadPath * normalize(g_Textures[g_iNormal_NaturalRoadPath].Sample(g_LinearSampler, In.vBaseTexcoord))) +
        (vLayer_RiverBed * normalize(g_Textures[g_iNormal_RiverBed].Sample(g_LinearSampler, In.vBaseTexcoord))) +
        (vLayer_SandA * normalize(g_Textures[g_iNormal_SandA].Sample(g_LinearSampler, In.vBaseTexcoord))) +
        (vLayer_SandC * normalize(g_Textures[g_iNormal_SandC].Sample(g_LinearSampler, In.vBaseTexcoord)));
    
    vNormal = normalize(vNormal);
    float3x3 matNormalWorld = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);
    vNormal = vector(vNormal.xyz * 2.f - 1.f, 0);
    vNormal = vector(mul(vNormal.xyz, matNormalWorld), 0);
    vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
    
    vRSAO =
        (vLayer_Cliff * g_Textures[g_iMaCliff].Sample(g_LinearSampler, vRotated)) +
        (vLayer_CoastBeach * g_Textures[g_iMaCoastBeach].Sample(g_LinearSampler, vRotated)) +
        (vLayer_RiverBed * g_Textures[g_iMaRiverBed].Sample(g_LinearSampler, vRotated)) +
        ((1 - (vLayer_Cliff + vLayer_CoastBeach + vLayer_RiverBed)) * vRSAO);
       
    Out.vDiffuse = vDiffuse;
    Out.vNormal = normalize(vNormal);
    Out.vEmission = vEmission;
    Out.fDepth = (In.vProjPosition.z / In.vProjPosition.w);
    Out.vWorldPosition = In.vWorldPosition;
   
    return Out;
};

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