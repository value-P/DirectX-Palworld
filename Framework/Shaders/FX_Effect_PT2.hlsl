#include "Shader_Defines.hlsli"

struct FEffectInfo_Deffered
{
    Matrix matWorld;

    Matrix matView;

    Matrix matProj;

    float fDistortionTimer;
    float3 vEffectDefferedPad1;
};

FEffectInfo_Deffered g_tEffectDeffered;
Texture2D g_Textures[9];

static const int g_iGlowDiffuse = 0;
static const int g_iBloom = 1;
static const int g_iBlurX = 2;
static const int g_iBlurY = 3;
static const int g_iBackBuffer = 4;
static const int g_iGlowResult = 5;
static const int g_iDistortion = 6;
static const int g_iMosaic = 7;
static const int g_iMosaicResult = 8;

static const float fWINCX = 1280.f;
static const float fWINCY = 720.f;

/* Blur */
static const float fWeight[13] =
{
    0.0561, 0.1353, 0.278, 0.4868, 0.7261, 0.9231, 1,
	0.9231, 0.7261, 0.4868, 0.278, 0.1353, 0.0561
};
static const float fTotal = 6.2108;

static const float fWeight2[17] =
{
    0.0561, 0.1353, 0.278, 0.4868, 0.6534, 0.7261, 0.8253, 0.9231, 1,
	0.9231, 0.8253, 0.7261, 0.6534, 0.4868, 0.278, 0.1353, 0.0561
};

static const float WeightSum = 9.1682;
static const int WeightCount = 8;

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

    Matrix matWV = mul(transpose(g_tEffectDeffered.matWorld), transpose(g_tEffectDeffered.matView));
    Matrix matWVP = mul(matWV, transpose(g_tEffectDeffered.matProj));
	
    Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
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
    float4 vColor : SV_TARGET0;
    float4 vCopy : SV_TARGET1;
};

PS_OUT PS_MAIN(PS_IN In) // 0
{
    PS_OUT Out = (PS_OUT) 0;
	
    vector vBackBuffer = g_Textures[g_iBackBuffer].Sample(g_LinearSampler, In.vTexcoord);
    vector vBlendTexture = g_Textures[g_iGlowResult].Sample(g_LinearSampler, In.vTexcoord);
	
    vector vFinal = vBackBuffer + vBlendTexture;
	
    Out.vColor = vFinal;
    Out.vCopy = vFinal;
	
    return Out;
}

PS_OUT PS_BLOOM(PS_IN In) // 2
{
    PS_OUT Out = (PS_OUT) 0;
	
    vector vBrightColor = 0.f;
    vector vFragColor = g_Textures[g_iGlowDiffuse].Sample(g_LinearSampler, In.vTexcoord);
    vFragColor.a = 1.f;
	
    float fBrightness = dot(vFragColor.rgb, float3(0.2126f, 0.7152f, 0.0722f));
    if (fBrightness > 0.6f)
    {
        vBrightColor = vector(vFragColor.rgb, 1.f);
    }
	
    Out.vColor = vBrightColor;
	
    return Out;
}

float4 Blur_X(float2 vTexCoord)
{
    PS_OUT Out = (PS_OUT) 0;
	
    float2 vTexUVOffset = 0;
    float texelSizeX = 1 / fWINCX; /* Get the size of a Texel Horizontally. */

    for (int i = -WeightCount; i < WeightCount; ++i)
    {
        vTexUVOffset = vTexCoord + float2(texelSizeX * i * 1.5f, 0); /* Get the UV coordinates for the Offsetted Pixel. */
        Out.vColor += fWeight2[WeightCount + i] * g_Textures[g_iBloom].Sample(g_LinearSampler, vTexUVOffset); /* Multiply the Pixel Color with his corresponding Weight and add it to the final Color. */
    }

    Out.vColor /= WeightSum; /* Average the final Color by the Weight Sum. */
    Out.vColor.a = 0.f;
    return Out.vColor;
}
float4 Blur_Y(float2 vTexCoord)
{
    PS_OUT Out = (PS_OUT) 0;
	
    float2 vTexUVOffset = 0;
    float texelSizeY = 1 / fWINCY; /* Get the size of a Texel Vertically. */

    for (int i = -WeightCount; i < WeightCount; ++i)
    {
        vTexUVOffset = vTexCoord + float2(0, texelSizeY * i * 1.5f); /* Get the UV coordinates for the Offsetted Pixel. */
        Out.vColor += fWeight2[WeightCount + i] * g_Textures[g_iBlurX].Sample(g_LinearSampler, vTexUVOffset); /* Multiply the Pixel Color with his corresponding Weight and add it to the final Color. */
    }

    Out.vColor /= WeightSum; /* Average the final Color by the Weight Sum. */
    Out.vColor.a = 0.f;
    return Out.vColor;
}

PS_OUT PS_BLUR_X(PS_IN In) // 3
{
    PS_OUT Out = (PS_OUT) 0;
	
    Out.vColor = Blur_X(In.vTexcoord);
	
    return Out;
}

PS_OUT PS_BLUR_Y(PS_IN In) // 4
{
    PS_OUT Out = (PS_OUT) 0;
	
    Out.vColor = Blur_Y(In.vTexcoord);
	
    return Out;
}

PS_OUT PS_GLOW(PS_IN In) // 5
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vBackBuffer = g_Textures[g_iBackBuffer].Sample(g_LinearSampler, In.vTexcoord);
    vector vBlurYTexture = g_Textures[g_iBlurY].Sample(g_LinearSampler, In.vTexcoord);
    vector vBloomTexture = g_Textures[g_iBloom].Sample(g_LinearSampler, In.vTexcoord);
    vector vHDRColor = g_Textures[g_iGlowDiffuse].Sample(g_LinearSampler, In.vTexcoord);
	
    float4 vBloom = pow(pow(abs(vBlurYTexture), 2.2f) + pow(abs(vBloomTexture), 2.2f), 1.f / 2.2f);
	
    vector vResult = vHDRColor;
	
    vResult = pow(abs(vResult), 2.2f);
    vBloom = pow(abs(vBloom), 2.2f);
	
    vResult += vBloom;
	
    Out.vColor = vBackBuffer + pow(abs(vResult), 1 / 2.2f);
	
    return Out;
}

PS_OUT PS_DISTORTION(PS_IN In) // 6
{
    PS_OUT Out = (PS_OUT) 0;
	
    vector vDistortionTex = g_Textures[g_iDistortion].Sample(g_LinearSampler, In.vTexcoord);
	
    if (0.f == vDistortionTex.r)
        discard;
	
    float2 vDistortedUV = In.vTexcoord + vDistortionTex.r + g_tEffectDeffered.fDistortionTimer;
	
    vector vBackBuffer = g_Textures[g_iBackBuffer].Sample(g_LinearSampler, vDistortedUV);
	
    Out.vColor = vBackBuffer;
	
    return Out;
}

PS_OUT PS_RADIAL_BLUR(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    return Out;
}

PS_OUT PS_POST_PROCESSING(PS_IN In) // 1
{
    PS_OUT Out = (PS_OUT) 0;
	
    vector vBackBuffer = g_Textures[g_iBackBuffer].Sample(g_LinearSampler, In.vTexcoord);
    vector vMosaic = g_Textures[g_iMosaicResult].Sample(g_LinearSampler, In.vTexcoord);
    
    vector vFinal = vBackBuffer + vMosaic;
	
    Out.vColor = vFinal;
    Out.vCopy = vFinal;
    
    return Out;
}

PS_OUT PS_MOSAIC(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float2 uv = In.vTexcoord;
    
    // uv.y = (1.f - uv.y);
    
    // uv = floor(uv * 20.f) / 20.f;
    
    Out.vColor = g_Textures[g_iMosaic].Sample(g_LinearSampler, uv);
    Out.vCopy = g_Textures[g_iMosaic].Sample(g_LinearSampler, uv);
    
    Out.vColor = float4(1.f, 1.f, 1.f, 1.f);
    Out.vCopy = float4(1.f, 1.f, 1.f, 1.f);
    
    return Out;
}

technique11 DefaultTechnique
{
    pass Default // 0
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_ZTestAndWrite, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass Post_Processing // 1
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_ZTestAndWrite, 0);
        SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_POST_PROCESSING();
    }

    pass Bloom // 2
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_ZTestAndWrite, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_BLOOM();
    }

    pass Blur_X // 3
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_ZTestAndWrite, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_BLUR_X();
    }

    pass Blur_Y // 4
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_ZTestAndWrite, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_BLUR_Y();
    }
    pass Glow // 5
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_ZTestAndWrite, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_GLOW();
    }

    pass Distortion // 6
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_ZTestAndWrite, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_DISTORTION();
    }

    pass RadialBlur // 7
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_ZTestAndWrite, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_RADIAL_BLUR();
    }

    pass Mosaic // 8
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MOSAIC();
    }
}


