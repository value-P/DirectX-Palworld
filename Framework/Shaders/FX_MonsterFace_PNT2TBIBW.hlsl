#include "Shader_Defines.hlsli"

struct FMeshInfo_MonsterFace
{
    matrix matBones[256];
    
    matrix matParentWorld;
    
    int iMaxIndexX;
    int iIndexX;
    int iMaxIndexY;
    int iIndexY;
};

FMeshInfo_MonsterFace g_tMeshInfo;
Texture2D g_Textures[1];
static const int g_iDiffuse = 0;

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

    float2 vFaceTexcoord = float2(0, 0);
    vFaceTexcoord.x = ((1.f / g_tMeshInfo.iMaxIndexX) * g_tMeshInfo.iIndexX) + In.vTexcoord.x;
    vFaceTexcoord.y = ((1.f / g_tMeshInfo.iMaxIndexY) * g_tMeshInfo.iIndexY) + In.vTexcoord.y;
    
    vector vDiffuse = vector(1, 0, 1, 1);
    vector vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0);
    vector vMRSO = vector(0, 0.04f, 0, 1);
    vector vEmission = vector(0, 0, 0, 0);
    
    vDiffuse = g_Textures[g_iDiffuse].Sample(g_LinearSampler, vFaceTexcoord);
    
    if (vDiffuse.a <= 0.1f)
    {
        discard;
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