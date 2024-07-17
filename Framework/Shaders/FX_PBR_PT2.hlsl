#include "Shader_Defines.hlsli"

struct FDirectionalLight
{
    float3 vDirection; // 광원의 방향, 단위: 없음, 범위: -1.0 ~ 1.0 (정규화된 벡터)
    float fIntensity; // 광원의 강도, 단위: 없음, 범위: 0.0 ~ 무한대 (일반적으로 0.0 ~ 1.0)

    float3 vColor; // 광원의 색상, 단위: 없음, 범위: 0.0 ~ 1.0 (RGB 색상 공간)
    float pad1;
};

struct FPointLight
{
    float3 vPosition; // 광원의 위치, 단위: 월드 유닛 (예: 미터), 범위: 실수 전체
    float fIntensity; // 광원의 강도, 단위: 없음, 범위: 0.0 ~ 무한대 (일반적으로 0.0 ~ 1.0)

    float3 vColor; // 광원의 색상, 단위: 없음, 범위: 0.0 ~ 1.0 (RGB 색상 공간)
    float fRadius; // 광원의 반경, 단위: 월드 유닛 (예: 미터), 범위: 0.0 ~ 무한대
    
    float fFalloff; // 광원의 감쇠율, 단위: 없음, 범위: 0.0 ~ 무한대 (일반적으로 0.0 ~ 1.0)
    float3 pad1;
};

struct FAmbientLight
{
    float3 vColor; // 광원의 색상, 단위: 없음, 범위: 0.0 ~ 1.0 (RGB 색상 공간)
    float fIntensity; // 광원의 강도, 단위: 없음, 범위: 0.0 ~ 무한대 (일반적으로 0.0 ~ 1.0)
};

struct FLightInfo
{
    FDirectionalLight tDirectionalLight;
    FPointLight tPointLights[8];
    uint iPointLightCount;
    FAmbientLight tAmbientLight;
};

struct FPBR
{
    Matrix matWorld;
    Matrix matView;
    Matrix matProj;
    FLightInfo tLightInfo;
};

FPBR g_tPBR;
Texture2D g_Textures[6];

static const int g_iDiffuse = 0;
static const int g_iNormal = 1;
static const int g_iMRSO = 2;
static const int g_iEmission = 3;
static const int g_iDepth = 4;
static const int g_iWorldPosition = 5;

float3 CalcDirLight(float3 _vDiffuse, float3 _vNormal, float4 _vMRSO, float3 _vViewDir);

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

    matrix matWV, matWVP;

    matWV = mul(transpose(g_tPBR.matWorld), transpose(g_tPBR.matView));
    matWVP = mul(matWV, transpose(g_tPBR.matProj));

    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
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
    
    float4 vDiffuse = g_Textures[g_iDiffuse].Sample(g_LinearSampler, In.vTexcoord);
    float4 vMRSO = g_Textures[g_iMRSO].Sample(g_LinearSampler, In.vTexcoord);
    float4 vNormal = g_Textures[g_iNormal].Sample(g_LinearSampler, In.vTexcoord);
    float4 vEmission = g_Textures[g_iEmission].Sample(g_LinearSampler, In.vTexcoord);
    float fDepth = g_Textures[g_iDepth].Sample(g_LinearSampler, In.vTexcoord).r;
    float4 vWorldPosition = g_Textures[g_iWorldPosition].Sample(g_LinearSampler, In.vTexcoord);
    vWorldPosition.w = 1.f;
 
    float3 vViewDirection = normalize(g_tCamera.vCameraPosition - vWorldPosition.xyz);
    
    float4 vGammaCorrected = pow(vDiffuse, 2.2f);
    
    vGammaCorrected = float4(CalcDirLight(vGammaCorrected.xyz, vNormal.xyz, vMRSO, vViewDirection), 1);
    
    vGammaCorrected += clamp(vEmission, 0.f, 1.f);
    
    float4 vInverseGammaCorrect = pow(vGammaCorrected, 1.f / 2.2f);
    
    Out.vColor = float4(vInverseGammaCorrect.xyz, vDiffuse.w);
    Out.vCopy = Out.vColor;
      
    if (g_tFrame.iDebugRenderTarget == 2)
    {
        Out.vColor = vDiffuse;
        Out.vCopy = vDiffuse;
    }
    else if (g_tFrame.iDebugRenderTarget == 3)
    {
        Out.vColor = vNormal;
        Out.vCopy = vNormal;
    }
    else if (g_tFrame.iDebugRenderTarget == 4)
    {
        Out.vColor = vMRSO;
        Out.vCopy = vMRSO;
    }
    else if (g_tFrame.iDebugRenderTarget == 5)
    {
        Out.vColor = vMRSO.r;
        Out.vCopy = vMRSO.r;
    }
    else if (g_tFrame.iDebugRenderTarget == 6)
    {
        Out.vColor = vMRSO.g;
        Out.vCopy = vMRSO.g;
    }
    else if (g_tFrame.iDebugRenderTarget == 7)
    {
        Out.vColor = vMRSO.b;
        Out.vCopy = vMRSO.b;
    }
    else if (g_tFrame.iDebugRenderTarget == 8)
    {
        Out.vColor = vMRSO.a;
        Out.vCopy = vMRSO.a;
    }
    else if (g_tFrame.iDebugRenderTarget == 9)
    {
        Out.vColor = vEmission;
        Out.vCopy = vEmission;
    }
    else if (g_tFrame.iDebugRenderTarget == 10)
    {
        Out.vColor = fDepth;
        Out.vCopy = fDepth;
    }
    
    return Out;
}

technique11 DefaultTechnique
{
    pass Default
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_ZTestAndWrite, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }
}

// a : 표면의 거칠기 (Roughness)
// n : 표면의 노말 벡터 (Normal)
// h : 빛과 시점 사이의 반반사 벡터 (Half Vector)
float NormalDistributionGGXTR(float3 n, float3 h, float a)
{
    float a2 = a * a; // 거칠기의 제곱
    float NdotH = saturate(dot(n, h)); // 노말 벡터와 반반사 벡터의 내적
    float NdotH2 = NdotH * NdotH; // 내적의 제곱

    float nom = a2; // 분자
    float denom = (NdotH2 * (a2 - 1.0f) + 1.0f); // 분모
    denom = PI * denom * denom; // 분모에 파이를 곱하고 제곱

    return nom / denom; // GGX-Trowbridge-Reitz NDF를 계산하여 반환
}

// NdotV : 노말 벡터와 시점 벡터의 내적
// roughness : 표면의 거칠기
float GeometrySchlickGGX(float NdotV, float roughness)  // k is a remapping of roughness based on direct lighting or IBL lighting
{
    // 거칠기를 재매핑
    float r = roughness + 1.0f;
    float k = (r * r) / 8.0f;

    // Schlick-GGX 기하학 항의 분자와 분모를 계산
    float nom = NdotV;
    float denom = NdotV * (1.0f - k) + k;

    // Schlick-GGX 기하학 항을 계산하여 반환
    return nom / denom;
}

// n : 표면의 노말 벡터 (Normal)
// v : 시점 벡터 (View Vector)
// l : 빛의 방향 벡터 (Light Vector)
// k : 표면의 거칠기 (Roughness)
float GeometrySmith(float3 n, float3 v, float3 l, float k)
{
    // 노말 벡터와 시점 벡터의 내적을 계산
    float NdotV = saturate(dot(n, v));

    // 노말 벡터와 빛의 방향 벡터의 내적을 계산
    float NdotL = saturate(dot(n, l));

    // 시점 벡터와 빛의 방향 벡터에 대한 Schlick-GGX 기하학 항을 계산
    float ggx1 = GeometrySchlickGGX(NdotV, k);
    float ggx2 = GeometrySchlickGGX(NdotL, k);

    // 최종 기하학 항을 계산하여 반환
    return ggx1 * ggx2;
}

// cosTheta : 빛과 시점 사이의 각도의 코사인 값
// F0 : 표면의 Fresnel reflectance at normal incidence (표면이 빛을 얼마나 잘 반사하는지를 나타내는 값)
float3 FresnelSchlick(float cosTheta, float3 F0)
{
    // Fresnel-Schlick 방정식을 계산하여 반환
    return (F0 + (1.0f - F0) * pow(1.0 - cosTheta, 5.0f));
}

float3 CalcDirLight(float3 _vDiffuse, float3 _vNormal, float4 _vMRSO, float3 _vViewDir)
{
    float3 F0 = 0.04f; //  F0 : reflectance ratio - 0.04 looks visually correct for non-metallic sufaces
    F0 = lerp(F0, _vDiffuse, _vMRSO.r);
    
    float3 lightDir = -g_tPBR.tLightInfo.tDirectionalLight.vDirection;
    float3 halfwayVec = normalize(_vViewDir + lightDir);
    float3 radiance = g_tPBR.tLightInfo.tDirectionalLight.vColor;

//  Cook-Torrance specular BRDF
    float D = NormalDistributionGGXTR(_vNormal, halfwayVec, _vMRSO.g);
    float G = GeometrySmith(_vNormal, _vViewDir, lightDir, _vMRSO.g);
    float3 F = FresnelSchlick(max(dot(halfwayVec, _vViewDir), 0.0f), F0);

    float3 nominator = D * G * F;
//  Wo : View Direction
//  Wi : Light Direction
    float WoDotN = saturate(dot(_vViewDir, _vNormal));
    float WiDotN = saturate(dot(lightDir, _vNormal));
    float denominator = (4 * WoDotN * WiDotN);

// 0.001f just in case product is 0
    float3 specular = nominator / (denominator + 0.001f);

//  Energy Conservation
    float3 kS = F; //  reflection energy
    float3 kD = 1.0f - kS; //  refraction energy
    kD *= 1.0 - _vMRSO.r; //  if metallic nullify kD

//  Calculate radiance
    return (((kD * _vDiffuse / PI) + specular) * radiance * WiDotN);
}