#include "Shader_Defines.hlsli"

struct FMeshInfo_Effect
{
    Matrix matGroupWorld;

    Matrix matWorld;

    bool bDiffuse;
    bool bNoise;
    bool bMask;
    bool bDistortion;

    bool bDissolve;
    float fTrackPosition;
    float2 vLifeTime;

    bool bUVMoveLoop;
    float2 vUVAcc;
    bool bSprite;

    float2 vMinUV;
    float2 vMaxUV;

    bool bGlow;
    float fGlowWeight;
    bool bBloom;
    float fBloomWeight;

    bool bAlphaAttenuation;
    float fAlphaAttenuationWeight;
    float fDistortionWeight;
    float fDissolveWeight;

    float4 vDiffuseColor1;

    float4 vColor1;

    float4 vColor2;

    float4 vColor3;
    
    bool bTrail;
    
    float fAlphaDiscardValue;
};

FMeshInfo_Effect g_tMeshInfo;
Texture2D g_Textures[5];

/* 텍스처 */
static const int g_iDiffuse = 0;
static const int g_iNoise = 1;
static const int g_iMask = 2;
static const int g_iDistortion = 3;
static const int g_iDissolve = 4;

struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float3 vTangent : TANGENT;
	
    float4 vRight : WORLD0;
    float4 vUp : WORLD1;
    float4 vLook : WORLD2;
    float4 vTranslation : WORLD3;
    float4 vPrevTranslation : WORLD4;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
    
    Matrix matWorld = transpose(g_tMeshInfo.matWorld);

    matrix matWV, matVP, matWVP;
    matWV = mul(matWorld, g_tCamera.matView);
    matVP = mul(g_tCamera.matView, g_tCamera.matProj);
    matWVP = mul(matWV, g_tCamera.matProj);

    matrix matTrans;
    matTrans[0] = In.vRight;
    matTrans[1] = In.vUp;
    matTrans[2] = In.vLook;
    matTrans[3] = In.vTranslation;
	
    Out.vPosition = mul(vector(In.vPosition, 1.f), matTrans);
    Out.vPosition = mul(Out.vPosition, transpose(g_tMeshInfo.matGroupWorld));
    Out.vPosition = mul(Out.vPosition, matVP);
    
    Out.vTexcoord.x = In.vTexcoord.x + g_tMeshInfo.vUVAcc.x;
    Out.vTexcoord.y = In.vTexcoord.y + g_tMeshInfo.vUVAcc.y;
    Out.vNormal = mul(vector(In.vNormal, 0.f), matWorld);
    
    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
};

struct PS_OUT
{
    vector vColor : SV_TARGET0;
    vector vCopy : SV_TARGET1;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
	
    // 스프라이트
    if (g_tMeshInfo.bSprite)
    {
        if ((In.vTexcoord.x < g_tMeshInfo.vMinUV.x || g_tMeshInfo.vMaxUV.x < In.vTexcoord.x) &&
        (In.vTexcoord.y < g_tMeshInfo.vMinUV.y || g_tMeshInfo.vMaxUV.y < In.vTexcoord.y))
            discard;
    }
    
    // 알파 감쇄
    float fDistanceToHalf;
    if (g_tMeshInfo.bAlphaAttenuation)
    {
        if (In.vTexcoord.y < 0.5f)
            fDistanceToHalf = In.vTexcoord.y;
        else
            fDistanceToHalf = 1.f - In.vTexcoord.y;
        
        if (fDistanceToHalf < saturate((g_tMeshInfo.fTrackPosition - g_tMeshInfo.vLifeTime.x) / (g_tMeshInfo.vLifeTime.y - g_tMeshInfo.vLifeTime.x)) * g_tMeshInfo.fAlphaAttenuationWeight)
            discard;
    }
    
	// 디퓨즈
    vector vDiffuse1;
    vector vDiffuse2;
    vector vDiffuse3;
    if (g_tMeshInfo.bDiffuse)
    {
        vDiffuse1 = g_Textures[g_iDiffuse].Sample(g_LinearSampler, In.vTexcoord);
    }
    else
    {
        vDiffuse1 = g_tMeshInfo.vColor1;
        vDiffuse2 = g_tMeshInfo.vColor2;
        vDiffuse3 = g_tMeshInfo.vColor3;
    }
	
// 노이즈
    vector vNoise;
    float fNoiseValue = 1.f;
    if (g_tMeshInfo.bNoise)
    {
        vNoise = g_Textures[g_iNoise].Sample(g_LinearSampler, In.vTexcoord);
        float fNoiseValue = vNoise.r;
		
        if (0.5f < fNoiseValue)
            discard;
    }
    else
    {
        vNoise = vector(1.f, 1.f, 1.f, 1.f);
    }
	
	// 마스크
    vector vMask;
    float fMaskValue = 1.f;
    if (g_tMeshInfo.bMask)
    {
        vMask = g_Textures[g_iMask].Sample(g_LinearSampler, In.vTexcoord);
    }
    else
    {
        vMask = vector(1.f, 1.f, 1.f, 1.f);
    }
    fMaskValue = vMask.r;
    fMaskValue *= vMask.a;
	
    if (0.1f > fMaskValue)
        discard;

    vector vFinalDiffuse = vDiffuse1 * fMaskValue * fNoiseValue;
	
    // if (0.f == vFinalDiffuse.a)
    //     discard;
	
    Out.vColor = vFinalDiffuse;
    Out.vCopy = vFinalDiffuse;
	
    return Out;
}

technique11 DefaultTechnique
{
    pass DefaultPass
    {
        SetRasterizerState(RS_None_Cull);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }
}