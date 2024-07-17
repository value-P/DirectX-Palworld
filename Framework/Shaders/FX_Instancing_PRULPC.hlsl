#include "Shader_Defines.hlsli"

struct FEffectInfo_Particle
{
    Matrix matWorld;

    bool bDiffuse;
    bool bMask;
    bool bBillboard;
    bool bDirectional;

    float fSizeRatio;
    float2 vSize;
    float fParticlePad1;

    float4 vColor1;

    float4 vColor2;

    float4 vColor3;
    
    float fTrackPosition;
};

FEffectInfo_Particle g_tEffectInfo_Particle;
Texture2D g_Textures[2];

static const int g_iDiffuse = 0;
static const int g_iMask = 1;

struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vPSize : PSIZE;

    float4 vRight : WORLD0;
    float4 vUp : WORLD1;
    float4 vLook : WORLD2;
    float4 vTranslation : WORLD3;
    float4 vPrevTranslation : WORLD4;

    float4 vColor : COLOR0;
	
    float2 vMinUV : TEXCOORD0;
    float2 vMaxUV : TEXCOORD1;
};

struct VS_OUT
{
    float4 vPosition : POSITION;
    float2 vPSize : PSIZE;
	
    float4 vRight : WORLD0;
    float4 vUp : WORLD1;
    float4 vLook : WORLD2;
    float4 vTranslation : WORLD3;
    float4 vPrevTranslation : WORLD4;
	
    float4 vColor : COLOR0;
	
    float2 vMinUV : TEXCOORD0;
    float2 vMaxUV : TEXCOORD1;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    matrix matTrans;
    matTrans[0] = In.vRight;
    matTrans[1] = In.vUp;
    matTrans[2] = In.vLook;
    matTrans[3] = In.vTranslation;
	
    Out.vPosition = In.vTranslation;
	
    Out.vPSize = float2(length(matTrans._11_12_13) * In.vPSize.x, length(matTrans._21_22_23) * In.vPSize.y);
	
    Out.vRight = In.vRight;
    Out.vUp = In.vUp;
    Out.vLook = In.vLook;
    Out.vTranslation = In.vTranslation;
    Out.vPrevTranslation = In.vPrevTranslation;
	
	// Out.vColor는 Bind로 던져준 DiffuseColor1을 사용한다.
    Out.vColor = In.vColor;
	
	// 스프라이트용 Min, Max UV를 파티클 컴포넌트에서 던져준다.
    Out.vMinUV = In.vMinUV;
    Out.vMaxUV = In.vMaxUV;

    return Out;
}

struct GS_IN
{
    float4 vPosition : POSITION;
    float2 vPSize : PSIZE;
	
    float4 vRight : WORLD0;
    float4 vUp : WORLD1;
    float4 vLook : WORLD2;
    float4 vTranslation : WORLD3;
    float4 vPrevTranslation : WORLD4;
	
    float4 vColor : COLOR0;
	
    float2 vMinUV : TEXCOORD0;
    float2 vMaxUV : TEXCOORD1;
};

struct GS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float4 vColor : COLOR0;
};

[maxvertexcount(6)]
void GS_MAIN(point GS_IN In[1], inout TriangleStream<GS_OUT> DataStream)
{
    GS_OUT Out[4];

    matrix matVP = mul(g_tCamera.matView, g_tCamera.matProj);

    float3 vCamPos = g_tCamera.vCameraPosition;
    float3 vRight = float3(1.f, 0.f, 0.f);
    float3 vUp = float3(0.f, 1.f, 0.f);
    float3 vLook = float3(0.f, 0.f, 1.f);
	
    if (g_tEffectInfo_Particle.bBillboard)
    {
        if (g_tEffectInfo_Particle.bDirectional)
        {
			// 인스턴스의 방향 벡터
            float3 vMoveDir = normalize((In[0].vTranslation - In[0].vPrevTranslation).xyz);
			
            float3 vPlaneNormal = normalize(vCamPos - In[0].vTranslation.xyz);
            float3 vPointOnPlane = In[0].vTranslation.xyz;
			
			// 평면의 방정식 ax + by + cz + d = 0
            float3 vMovedPoint = vPointOnPlane + vMoveDir;
            float d = -(vPointOnPlane.x * vPlaneNormal.x + vPointOnPlane.y * vPlaneNormal.y + vPointOnPlane.z * vPlaneNormal.z);
            float fResult = vMovedPoint.x * vPlaneNormal.x + vMovedPoint.y * vPlaneNormal.y + vMovedPoint.z * vPlaneNormal.z + d;
			
            float fNormalToPoint = dot(vPointOnPlane, vPlaneNormal);
			
            float fDistance = dot(vMoveDir, vPlaneNormal) - fNormalToPoint;
			
			// 사영 시킨 벡터
            float3 vProjectedDir = vMoveDir - vPlaneNormal;
			
            vLook = vPlaneNormal;
            vUp = normalize(vProjectedDir) * In[0].vPSize.x * g_tEffectInfo_Particle.fSizeRatio * 0.5f;
            vRight = normalize(cross(vLook, vUp)) * In[0].vPSize.y * g_tEffectInfo_Particle.fSizeRatio * 0.5f;
        }
        else
        {
            vLook = normalize((vCamPos - In[0].vTranslation.xyz).xyz);
            vRight = normalize(cross(float3(0.f, 1.f, 0.f), vLook)) * In[0].vPSize.x * g_tEffectInfo_Particle.fSizeRatio * 0.5f;
            vUp = normalize(cross(vLook, vRight)) * In[0].vPSize.y * g_tEffectInfo_Particle.fSizeRatio * 0.5f;
        }
    }
    else
    {
        if (g_tEffectInfo_Particle.bDirectional)
            vLook = normalize((In[0].vTranslation - In[0].vPrevTranslation).xyz);
        else
            vLook = float3(In[0].vLook.x, In[0].vLook.y, In[0].vLook.z);
		
        vRight = normalize(cross(float3(0.f, 1.f, 0.f), vLook)) * In[0].vPSize.x * g_tEffectInfo_Particle.fSizeRatio * 0.5f;
        vUp = normalize(cross(vLook, vRight)) * In[0].vPSize.y * g_tEffectInfo_Particle.fSizeRatio * 0.5f;
		
		// vRight	= float3(In[0].vRight.x, In[0].vRight.y, In[0].vRight.z) * g_fSizeRatio * 0.5f;
		// vUp		= float3(In[0].vUp.x, In[0].vUp.y, In[0].vUp.z) * g_fSizeRatio * 0.5f;
    }
	
	// 4. 사각형의 상, 하, 좌, 우 꼭짓점을 구한다.
    Out[0].vPosition = In[0].vPosition + float4(vRight, 0.f) + float4(vUp, 0.f);
    Out[0].vPosition = mul(Out[0].vPosition, matVP);
    Out[0].vTexcoord = float2(In[0].vMinUV.x, In[0].vMinUV.y);
    Out[0].vColor = In[0].vColor;

    Out[1].vPosition = In[0].vPosition - float4(vRight, 0.f) + float4(vUp, 0.f);
    Out[1].vPosition = mul(Out[1].vPosition, matVP);
    Out[1].vTexcoord = float2(In[0].vMaxUV.x, In[0].vMinUV.y);
    Out[1].vColor = In[0].vColor;

    Out[2].vPosition = In[0].vPosition - float4(vRight, 0.f) - float4(vUp, 0.f);
    Out[2].vPosition = mul(Out[2].vPosition, matVP);
    Out[2].vTexcoord = float2(In[0].vMaxUV.x, In[0].vMaxUV.y);
    Out[2].vColor = In[0].vColor;

    Out[3].vPosition = In[0].vPosition + float4(vRight, 0.f) - float4(vUp, 0.f);
    Out[3].vPosition = mul(Out[3].vPosition, matVP);
    Out[3].vTexcoord = float2(In[0].vMinUV.x, In[0].vMaxUV.y);
    Out[3].vColor = In[0].vColor;

	// 5. DataStream 컨테이너에 Out으로 내보낼 삼각형의 인덱스 순서대로 보관한다.
    DataStream.Append(Out[0]);
    DataStream.Append(Out[1]);
    DataStream.Append(Out[2]);
    DataStream.RestartStrip();

    DataStream.Append(Out[0]);
    DataStream.Append(Out[2]);
    DataStream.Append(Out[3]);
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float4 vColor : COLOR0;
};

struct PS_OUT
{
    float4 vColor : SV_TARGET0;
    float4 vCopy : SV_TARGET1;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
	
	// 디퓨즈
    vector vDiffuse1;
    vector vDiffuse2;
    vector vDiffuse3;
    if (g_tEffectInfo_Particle.bDiffuse)
    {
        vDiffuse1 = g_Textures[g_iDiffuse].Sample(g_LinearSampler, In.vTexcoord);
    }
    else
    {
        vDiffuse1 = In.vColor;
        vDiffuse2 = In.vColor;
        vDiffuse3 = In.vColor;
    }
    // vDiffuse1.a = g_tEffectInfo_Particle.vColor1.a;

	// 마스크
    vector vMask;
    float fMaskValue = 1.f;
    if (g_tEffectInfo_Particle.bMask)
    {
        vMask = g_Textures[g_iMask].Sample(g_LinearSampler, In.vTexcoord);
    }
    else
    {
        vMask = vector(1.f, 1.f, 1.f, 1.f);
    }
    fMaskValue = vMask.r;
    fMaskValue *= vMask.a;
	
    if (0.3f > fMaskValue)
        discard;
	
    vector vFinalColor = vDiffuse1 * fMaskValue;
	
    if (0.3f > vFinalColor.a)
        discard;
    
    Out.vColor = vFinalColor;
    Out.vCopy = vFinalColor;
	
    return Out;
}

float Firenoise(float3 p) //Thx to Las^Mercury
{
    float3 i = floor(p);
    float4 a = dot(i, float3(1., 57., 21.)) + float4(0., 57., 21., 78.);
    float3 f = cos((p - i) * acos(-1.0)) * (-0.5) + 0.5;
    a = lerp(sin(cos(a) * a), sin(cos(1.0 + a) * (1.0 + a)), f.x);
    a.xy = lerp(a.xz, a.yw, f.y);
    return lerp(a.x, a.y, f.z);
}

float sphere(float3 p, float4 spr)
{
    return length(spr.xyz - p) - spr.w;
}

float flame(float3 p)
{
    float d = sphere(p * float3(0.75, 0.6, 1.), float4(0.0, -1.0, 0.0, 1.0));
    return d + (Firenoise(p + float3(0.0, g_tEffectInfo_Particle.fTrackPosition * 8.0, 0.0)) + Firenoise(p * 3.0) * 0.5) * 0.25 * (p.y);
}

float scene(float3 p)
{
    return min(100.0 - length(p), abs(flame(p)));
}

float4 raymarch(float3 org, float3 dir)
{
    float d = 0.0, glow = 0.0, eps = 0.02;
    float3 p = org;
    bool glowed = false;
    
    for (int i = 0; i < 64; i++)
    {
        d = scene(p) + eps;
        p += d * dir;
        if (d > eps)
        {
            if (flame(p) < 0.0)
                glowed = true;
            if (glowed)
                glow = float(i) / 64.0;
        }
    }
    return float4(p, glow);
}

PS_OUT PS_FIRE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float2 v = -1.0 + 2.0 * float2(In.vTexcoord.x, 1.0 - In.vTexcoord.y);
    v *= 5.f;
    v.y -= 0.4f;
    
    float3 org = float3(0.0, -2.0, 4.0);
    float3 dir = normalize(float3(v.x * 1.2, -v.y, -1.0));
    
    float4 p = raymarch(org, dir);
    float glow = p.w;
    
    float4 col = lerp(float4(1.0, 0.5, 0.1, 1.0), float4(1.0, 0.5, 0.1, 1.0), p.y * 0.02 + 0.4);
    
    col =  lerp(float4(0.0, 0.0, 0.0, 0.0), col, pow(glow * 2.0, 4.0));
    
    if(0.1f > col.a)
        discard;
    
    Out.vColor = col;
    Out.vCopy = col;
    
    return Out;
}

#define FLAMECOLOR float3(50.0, 5.0, 1.0)
// #define PI 3.14159

float FlameShape(float2 uv)
{
    float flameControl1 = 4.5;
    float flameControl2 = 0.5;

    float a = fmod(atan2(uv.x, uv.y + 2.0), PI * 2.0) / flameControl1 - PI / flameControl1;
    float angle = PI - (0.5 + 0.25);
    float d = length(uv - float2(0.0, -2.0)) * sin(angle + abs(a));
    return smoothstep(0.0, flameControl2, d);
}

float2x2 Rot(float a)
{
    float s = sin(a), c = cos(a);
    return float2x2(c, -s, s, c);
}

float R21(float2 p)
{
    return frac(sin(dot(p, float2(2.3245, 5.234))) * 123.5632145);
}

float NoiseValue(float2 uv)
{
    float2 gv = frac(uv);
    float2 id = floor(uv);

    gv = gv * gv * (3.0 - 2.0 * gv);

    float a = R21(id);
    float b = R21(id + float2(1.0, 0.0));
    float c = R21(id + float2(0.0, 1.0));
    float d = R21(id + float2(1.0, 1.0));

    return lerp(a, b, gv.x) + (c - a) * gv.y * (1.0 - gv.x) + (d - b) * gv.x * gv.y;
}

float SmoothNoise(float2 uv)
{
    float value = 0.0;
    float amplitude = 0.5;

    for (int i = 0; i < 8; i++)
    {
        value += NoiseValue(uv) * amplitude;
        uv *= 2.0;
        amplitude *= 0.5;
    }

    return value;
}

float3 Flame2(float2 uv)
{
    uv *= 1.0;

    float3 col = float3(0.0, 0.0, 0.0);
    float shape = FlameShape(uv);

    uv = mul(uv, Rot(2.5));

    float2 rn = float2(0.0, 0.0);
    rn.x = SmoothNoise(uv + 1.984 + 4.5 * g_tEffectInfo_Particle.fTrackPosition);
    rn.y = SmoothNoise(uv + 1.0 + 4.5 * g_tEffectInfo_Particle.fTrackPosition);

    col += SmoothNoise(uv + rn * 2.5);
    col -= shape;

    col = col / (1.5 + col);
    col = pow(col, float3(3.0 / 2.2, 3.0 / 2.2, 3.0 / 2.2));

    col *= FLAMECOLOR;

    return col;
}

PS_OUT PS_FIRE2(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float2 uv = In.vTexcoord * float2(2.f, 2.f) - float2(1.f, 1.f);

    float3 col = Flame2(uv);
    
    Out.vColor = float4(col, 1.f);
    Out.vCopy = float4(col, 1.f);

    return Out;
}

PS_OUT PS_NONE_DISCARD(PS_IN In) // 3
{
    PS_OUT Out = (PS_OUT) 0;
	
	// 디퓨즈
    vector vDiffuse1;
    vector vDiffuse2;
    vector vDiffuse3;
    if (g_tEffectInfo_Particle.bDiffuse)
    {
        vDiffuse1 = g_Textures[g_iDiffuse].Sample(g_LinearSampler, In.vTexcoord);
    }
    else
    {
        vDiffuse1 = In.vColor;
        vDiffuse2 = In.vColor;
        vDiffuse3 = In.vColor;
    }

	// 마스크
    vector vMask;
    float fMaskValue = 1.f;
    if (g_tEffectInfo_Particle.bMask)
    {
        vMask = g_Textures[g_iMask].Sample(g_LinearSampler, In.vTexcoord);
    }
    else
    {
        vMask = vector(1.f, 1.f, 1.f, 1.f);
    }
    fMaskValue = vMask.r;
    fMaskValue *= vMask.a;
	
    // if (0.3f > fMaskValue)
        // discard;
	
    vector vFinalColor = vDiffuse1 * fMaskValue;
	
    if (0.f == vFinalColor.a)
        discard;
    
    Out.vColor = vFinalColor;
    Out.vCopy = vFinalColor;
	
    return Out;
}

technique11 DefaultTechnique
{
    pass DefaultPass // 0
    {
        SetRasterizerState(RS_None_Cull);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN();
    }
    pass Fire // 1
    {
        SetRasterizerState(RS_None_Cull);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_FIRE();
    }
    pass Water // 2
    {
        SetRasterizerState(RS_None_Cull);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_FIRE2();
    }
    pass AlphaNoneDiscard // 3
    {
        SetRasterizerState(RS_None_Cull);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_NONE_DISCARD();
    }
}