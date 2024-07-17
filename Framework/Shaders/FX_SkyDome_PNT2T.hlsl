#include "Shader_Defines.hlsli"

struct FMeshInfo_SkyDome
{
	Matrix matTransform;

	Matrix matWorld;
};

FMeshInfo_SkyDome g_tMeshInfo;
Texture2D g_Textures[3];

static const int g_iDay = 0;
static const int g_iCloud = 1;
static const int g_iNight = 2;

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
	float2 vTexcoord : TEXCOORD0;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT Out = (VS_OUT)0;

	Matrix matTransform = transpose(g_tMeshInfo.matTransform);

	// 로컬 변환 행렬 적용
	float4 vLocalPosition = mul(float4(In.vPosition, 1), matTransform);
	vLocalPosition += float4(g_tCamera.vCameraPosition, 0);

	// 월드 행렬 ID 값 갱신
	matrix matWorld = transpose(g_tMeshInfo.matWorld);
	int iGameObjectID = matWorld._44;
	matWorld._44 = 1;

	// 월드 뷰 투영 행렬
	matrix matWVP = mul(mul(matWorld, g_tCamera.matView), g_tCamera.matProj);

	Out.vPosition = mul(vLocalPosition, matWVP);
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
	float4 vBackBuffer : SV_Target0;
	float4 vBackBuffer_Copy : SV_Target1;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	vector vDiffuse = vector(1, 1, 1, 1);

	float fTime = g_tFrame.fTotalTime % 120.f / 24.f;
	float fDayRatio = 0;
	float fNightRatio = 0;

	if (fTime >= 7.f && fTime < 18.f)
	{
		fDayRatio = 1.f;
		fNightRatio = 0.f;
	}
	else if (fTime >= 18.f && fTime < 20.f)
	{
		float fRatio = (20.f - fTime) / 2.f;
		fDayRatio = 1.f - fRatio;
		fNightRatio = fRatio;
	}
	else if (fTime >= 20.f || fTime < 5.f)
	{
		fDayRatio = 0.f;
		fNightRatio = 1.f;
	}
	else if (fTime >= 5.f && fTime < 7.f)
	{
		float fRatio = (7.f - fTime) / 2.f;
		fDayRatio = fRatio;
		fNightRatio = 1.f - fRatio;
	}

	float2 vTexCoord = In.vTexcoord + float2(g_tFrame.fTotalTime / 120.f, 0);

	vDiffuse = (fDayRatio * g_Textures[g_iDay].Sample(g_LinearSampler, vTexCoord)) + (fNightRatio * g_Textures[g_iNight].Sample(g_LinearSampler, vTexCoord));

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
}