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
Matrix g_matTransform;
Texture2D g_Textures[5];

static const int g_iDiffuse = 0;
static const int g_iNoise = 1;
static const int g_iMask = 2;
static const int g_iDistortion = 3;
static const int g_iDissolve = 4;

static const float2 g_iResolution = { 1280, 720 };

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
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT Out = (VS_OUT) 0;
 
	matrix matWV, matWVP;
	
	float4 vPosition = mul(float4(In.vPosition, 1), g_matTransform);

	Matrix matWorld = transpose(g_tMeshInfo.matWorld);

	matWV = mul(matWorld, g_tCamera.matView);
	matWVP = mul(matWV, g_tCamera.matProj);
	matrix matVP = mul(g_tCamera.matView, g_tCamera.matProj);
	
	Out.vPosition = mul(vPosition, matWorld);
	
	if(!g_tMeshInfo.bTrail)
	{
		Out.vPosition = mul(Out.vPosition, transpose(g_tMeshInfo.matGroupWorld));
	}
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
		vNoise = g_Textures[g_iNoise].Sample(g_LinearSampler, In.vTexcoord);
	else
		vNoise = vector(1.f, 1.f, 1.f, 1.f);
	
	// 마스크
	vector vMask;
	float fMaskValue = 1.f;
	if (g_tMeshInfo.bMask)
		vMask = g_Textures[g_iMask].Sample(g_LinearSampler, In.vTexcoord);
	else
		vMask = vector(1.f, 1.f, 1.f, 1.f);
	
	fMaskValue = vMask.r;
	fMaskValue *= vMask.a;
	
	if (g_tMeshInfo.fAlphaDiscardValue >= fMaskValue)
		discard;
	
	// 디스토션
	vector vDistortion;
	if (g_tMeshInfo.bDistortion)
	{
		float2 vDistortedTexcoord = In.vTexcoord + g_tMeshInfo.fDistortionWeight * g_Textures[g_iDistortion].Sample(g_LinearSampler, In.vTexcoord).xy;
		
		if (g_tMeshInfo.bDiffuse)
		{
			vDiffuse1 = g_Textures[g_iDiffuse].Sample(g_LinearSampler, vDistortedTexcoord);
		}
	}

	vector vFinalDiffuse = (vDiffuse1 * fMaskValue);
	
	if (0.f == vFinalDiffuse.a)
		discard;
	
	// 디졸브
	vector vDissolve;
	if (g_tMeshInfo.bDissolve)
	{
		// vDissolve = g_DissolveTexture.Sample(g_LinearSampler, In.vTexcoord * g_fDissolveWeight);
		float fTimeRatio = 1.f - ((g_tMeshInfo.fTrackPosition - g_tMeshInfo.vLifeTime.x) / g_tMeshInfo.vLifeTime.y);
		
		if (vNoise.r > fTimeRatio)
			discard;
		else
			vFinalDiffuse.a = 1.f;
		
		if (fTimeRatio - 0.1f < vNoise.r && vNoise.r + 0.1f)
		{
			vFinalDiffuse += float4(0.1f, 0.1f, 0.1f, 0.f);
		}
		else if (fTimeRatio - 0.05f < vNoise.r && vNoise.r + 0.05f)
		{
			vFinalDiffuse += float4(0.2f, 0.2f, 0.2f, 0.f);
		}
		else if (fTimeRatio - 0.02f < vNoise.r && vNoise.r + 0.02f)
		{
			vFinalDiffuse += float4(0.3f, 0.3f, 0.3f, 0.f);
		}
		//if (vDissolve.r >= (g_fTrackPosition / g_fMaxLifeTime))
		//{
			
		//}
	}
	else
	{
		vDissolve = (vector) 1.f;
	}
	
	Out.vColor = vFinalDiffuse;
	Out.vCopy = vFinalDiffuse;
	
	return Out;
}

float rand(float2 co)
{
    return frac(sin(dot(co.xy, float2(12.9898, 78.233))) * 43758.5453);
}

float hermite(float t)
{
    return t * t * (3.0 - 2.0 * t);
}

float noise(float2 co, float frequency)
{
    float2 v = float2(co.x * frequency, co.y * frequency);

    float ix1 = floor(v.x);
    float iy1 = floor(v.y);
    float ix2 = floor(v.x + 1.0);
    float iy2 = floor(v.y + 1.0);

    float fx = hermite(frac(v.x));
    float fy = hermite(frac(v.y));

    float fade1 = lerp(rand(float2(ix1, iy1)), rand(float2(ix2, iy1)), fx);
    float fade2 = lerp(rand(float2(ix1, iy2)), rand(float2(ix2, iy2)), fx);

    return lerp(fade1, fade2, fy);
}

float pnoise(float2 co, float freq, int steps, float persistence)
{
    float value = 0.0;
    float ampl = 1.0;
    float sum = 0.0;
    for (int i = 0; i < steps; i++)
    {
        sum += ampl;
        value += noise(co, freq) * ampl;
        freq *= 2.0;
        ampl *= persistence;
    }
    return value / sum;
}

PS_OUT PS_MAIN_PNOISED(PS_IN In)
{
	PS_OUT Out = (PS_OUT) 0;
	
    float2 uv = In.vTexcoord;
    float gradient = uv.y;
    float gradientStep = 0.2;
    
    float2 pos = In.vTexcoord;
    pos.y += g_tMeshInfo.fTrackPosition * 0.5;
    
    float4 brighterColor = float4(1.0, 0.65, 0.1, 1);
    float4 darkerColor = float4(1.0, 0.0, 0.15, 0);
    float4 middleColor = lerp(brighterColor, darkerColor, 0.5);

    float noiseTexel = pnoise(pos, 10.0, 5, 0.5);
    
    float firstStep = smoothstep(0.0, noiseTexel, gradient);
    float darkerColorStep = smoothstep(0.0, noiseTexel, gradient - gradientStep);
    float darkerColorPath = firstStep - darkerColorStep;
    float4 color = lerp(brighterColor, darkerColor, darkerColorPath);

    float middleColorStep = smoothstep(0.0, noiseTexel, gradient - 0.2 * 2.0);
    
    color = lerp(color, middleColor, darkerColorStep - middleColorStep);
    color = lerp(float4(0.f, 0.f, 0.f, 0.f), color, firstStep);
    float4 fragColor = color;
	
	// if(0.f >= fragColor.a)
        // discard;
	
    Out.vColor = fragColor;
    Out.vCopy = fragColor;
	
	return Out;
}

technique11 DefaultTechnique
{
	pass Default // 0
	{
		SetRasterizerState(RS_None_Cull);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	pass Noised // 1
	{
		SetRasterizerState(RS_None_Cull);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_PNOISED();
    }
}


