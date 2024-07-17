#include "Shader_Defines.hlsli"

struct FEffectInfo_Trail
{
    Matrix matWorld;

    bool bUseDiffuse;
    bool bUseNoise;
    bool bUseMask;
    bool bUseDistortion;

    bool bUseDissolve;
    float3 vTrailPad1;

    float4 vColor;
};

FEffectInfo_Trail g_tEffectInfo;
Texture2D g_Textures[5];

static const int g_iDiffuse = 0;
static const int g_iNoise = 1;
static const int g_iMask = 2;
static const int g_iDistortion = 3;
static const int g_iDissolve = 4;

struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    matrix matVP;

    matVP = mul(g_tCamera.matView, g_tCamera.matProj);
	
    Out.vPosition = mul(vector(In.vPosition, 1.f), matVP);
    Out.vTexcoord = In.vTexcoord;

    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
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
	
	// 디퓨즈
    vector vDiffuse;
    if (g_tEffectInfo.bUseDiffuse)
    {
        vDiffuse = g_Textures[g_iDiffuse].Sample(g_LinearSampler, In.vTexcoord);
    }
    else
    {
        vDiffuse = g_tEffectInfo.vColor;
    }
	
	// 노이즈
    vector vNoise;
    float fNoiseValue = 1.f;
    if (g_tEffectInfo.bUseNoise)
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
    if (g_tEffectInfo.bUseMask)
    {
        vMask = g_Textures[g_iMask].Sample(g_LinearSampler, In.vTexcoord);
    }
    else
    {
        vMask = vector(1.f, 1.f, 1.f, 1.f);
    }
    fMaskValue = vMask.r;
    fMaskValue *= vMask.a;
	
	// 디스토션
    vector vDistortion;
    if (g_tEffectInfo.bUseDistortion)
    {
        vDistortion = g_Textures[g_iDistortion].Sample(g_LinearSampler, In.vTexcoord);
    }
    else
    {
        vDistortion = vector(1.f, 1.f, 1.f, 1.f);
    }
	
	// 디졸브
    vector vDissolve;
    if (g_tEffectInfo.bUseDissolve)
    {
        vDissolve = g_Textures[g_iDissolve].Sample(g_LinearSampler, In.vTexcoord);
    }
    else
    {
        vDissolve = vector(1.f, 1.f, 1.f, 1.f);
    }

    vector vFinalDiffuse = vDiffuse * fMaskValue * fNoiseValue; // * vMask * vDistortion * vDissolve;
		
    if (0.f == vFinalDiffuse.a)
        discard;
	
    Out.vColor = vFinalDiffuse;
    Out.vCopy = vFinalDiffuse;
	
    return Out;
}

technique11 DefaultTechnique
{
    pass Default
    {
        SetRasterizerState(RS_None_Cull);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }
}


