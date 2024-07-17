#include "Shader_Defines.hlsli"

// UI - 기본
struct FUiInfo
{
    Matrix matWorld;

    Matrix matView;

    Matrix matProj;

    float fRatio;
    bool bAlpha;
    bool bColor;
    bool bMask;

    float fAlpha;
    float3 vRectpad1;

    float4 vColor;
};

// UI - AnimationImage
struct FUiInfo_AnimationImage
{
    float2 vMinTexCoord;
    float2 vMaxTexCoord;
};

// UI - ProgressBar
struct FUiInfo_ProgressBar
{
    float4 vOtherColor;

    float fOtherRatio;
    float3 vProgressBarPad1;
};

FUiInfo g_tUiInfo;
FUiInfo_AnimationImage g_tUiInfo_AnimationImage;
FUiInfo_ProgressBar g_tUiInfo_ProgressBar;
Texture2D g_Textures[2];

static const int g_iMask = 0;
static const int g_iBase = 1;

struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vPSize : PSIZE;
};

struct VS_OUT
{
    float4 vPosition : POSITION;
    float2 vPSize : PSIZE;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0.0f;
	
    Out.vPosition = vector(In.vPosition, 1.0f);
    Out.vPSize = float2(1.f, 1.f);
	
    return Out;
}

struct GS_IN
{
    float4 vPosition : POSITION;
    float2 vPSize : PSIZE;
};

struct GS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

[maxvertexcount(6)]
void GS_MAIN(point GS_IN In[1], inout TriangleStream<GS_OUT> DataStream)
{
    GS_OUT Out[4];

    matrix matWV = mul(transpose(g_tUiInfo.matWorld), transpose(g_tUiInfo.matView));
    matrix matWVP = mul(matWV, transpose(g_tUiInfo.matProj));

    Out[0].vPosition = In[0].vPosition + float4(-0.5f * In[0].vPSize.x, 0.5f * In[0].vPSize.y, 0.0f, 0.0f);
    Out[0].vPosition = mul(Out[0].vPosition, matWVP);
    Out[0].vTexcoord = float2(0.f, 0.f);
	// Out[0].vColor = In[0].vColor;

    Out[1].vPosition = In[0].vPosition + float4(0.5f * In[0].vPSize.x, 0.5f * In[0].vPSize.y, 0.0f, 0.0f);
    Out[1].vPosition = mul(Out[1].vPosition, matWVP);
    Out[1].vTexcoord = float2(1.f, 0.f);
	// Out[1].vColor = In[0].vColor;

    Out[2].vPosition = In[0].vPosition + float4(0.5f * In[0].vPSize.x, -0.5f * In[0].vPSize.y, 0.0f, 0.0f);
    Out[2].vPosition = mul(Out[2].vPosition, matWVP);
    Out[2].vTexcoord = float2(1.f, 1.f);
	// Out[2].vColor = In[0].vColor;

    Out[3].vPosition = In[0].vPosition + float4(-0.5f * In[0].vPSize.x, -0.5f * In[0].vPSize.y, 0.0f, 0.0f);
    Out[3].vPosition = mul(Out[3].vPosition, matWVP);
    Out[3].vTexcoord = float2(0.f, 1.f);
	// Out[3].vColor = In[0].vColor;

	// 5. DataStream 컨테이너에 Out으로 내보낼 삼각형의 인덱스 순서대로 보관한다.
    DataStream.Append(Out[0]);
    DataStream.Append(Out[1]);
    DataStream.Append(Out[2]);
    DataStream.RestartStrip();

    DataStream.Append(Out[0]);
    DataStream.Append(Out[2]);
    DataStream.Append(Out[3]);
}

[maxvertexcount(6)]
void GS_FRAMEANIMATION_MAIN(point GS_IN In[1], inout TriangleStream<GS_OUT> DataStream)
{
    GS_OUT Out[4];

    matrix matWV = mul(transpose(g_tUiInfo.matWorld), transpose(g_tUiInfo.matView));
    matrix matWVP = mul(matWV, transpose(g_tUiInfo.matProj));

    Out[0].vPosition = In[0].vPosition + float4(-0.5f * In[0].vPSize.x, 0.5f * In[0].vPSize.y, 0.0f, 0.0f);
    Out[0].vPosition = mul(Out[0].vPosition, matWVP);
    Out[0].vTexcoord = float2(g_tUiInfo_AnimationImage.vMinTexCoord.x, g_tUiInfo_AnimationImage.vMinTexCoord.y);

    Out[1].vPosition = In[0].vPosition + float4(0.5f * In[0].vPSize.x, 0.5f * In[0].vPSize.y, 0.0f, 0.0f);
    Out[1].vPosition = mul(Out[1].vPosition, matWVP);
    Out[1].vTexcoord = float2(g_tUiInfo_AnimationImage.vMaxTexCoord.x, g_tUiInfo_AnimationImage.vMinTexCoord.y);

    Out[2].vPosition = In[0].vPosition + float4(0.5f * In[0].vPSize.x, -0.5f * In[0].vPSize.y, 0.0f, 0.0f);
    Out[2].vPosition = mul(Out[2].vPosition, matWVP);
    Out[2].vTexcoord = float2(g_tUiInfo_AnimationImage.vMaxTexCoord.x, g_tUiInfo_AnimationImage.vMaxTexCoord.y);

    Out[3].vPosition = In[0].vPosition + float4(-0.5f * In[0].vPSize.x, -0.5f * In[0].vPSize.y, 0.0f, 0.0f);
    Out[3].vPosition = mul(Out[3].vPosition, matWVP);
    Out[3].vTexcoord = float2(g_tUiInfo_AnimationImage.vMinTexCoord.x, g_tUiInfo_AnimationImage.vMaxTexCoord.y);

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
};

struct PS_OUT
{
    vector vColor : SV_TARGET0;
};

PS_OUT PS_MAIN_STATIC(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0.0f;

    vector vDiffuse = g_Textures[g_iBase].Sample(g_LinearSampler, In.vTexcoord);
	
    if (g_tUiInfo.bColor)
    {
        vDiffuse.rgb = g_tUiInfo.vColor.rgb;
    }
	
    if (g_tUiInfo.bMask)
    {
        vector vMask = g_Textures[g_iMask].Sample(g_LinearSampler, In.vTexcoord);
		
        if (0.0f == vMask.a)
            discard;

        if (0.0f == vDiffuse.a)
            discard;
    }
    else
    {
        if (0.f == vDiffuse.a)
            discard;
    }
	
    Out.vColor = vDiffuse;
	
    if (g_tUiInfo.bAlpha)
    {
        Out.vColor.a = vDiffuse.a * g_tUiInfo.fAlpha;
    }
	
    return Out;
}

PS_OUT PS_MAIN_SLIDER(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0.0f;
	
    vector vDiffuse = g_Textures[g_iBase].Sample(g_LinearSampler, In.vTexcoord);
	
    if (g_tUiInfo.bColor)
    {
        vDiffuse.rgb = g_tUiInfo.vColor.rgb;
    }
	
    if (g_tUiInfo.bMask)
    {
        vector vMask = g_Textures[g_iMask].Sample(g_LinearSampler, In.vTexcoord);
		
        if (0.0f == vMask.a)
            discard;

        if (0.0f == vDiffuse.a)
        {
            vDiffuse = vMask;
        }
    }
    else
    {
        if (0.f == vDiffuse.a)
            discard;
    }
	
    if (In.vTexcoord.x > g_tUiInfo.fRatio)
    {
        vDiffuse.rgb = vector(0.5f, 0.5f, 0.5f, 1.0f);
    }
    
    Out.vColor = vDiffuse;
	
    if (g_tUiInfo.bAlpha)
    {
        Out.vColor.a = vDiffuse.a * g_tUiInfo.fAlpha;
    }
    
    return Out;
}

PS_OUT PS_MAIN_CIRCLEPROGRESS(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0.0f;
	
    vector vDiffuse =  g_Textures[g_iBase].Sample(g_LinearSampler, In.vTexcoord);
	
  //  if (0.f == vDiffuse.a)
		//discard;

    if (g_tUiInfo.bColor)
    {
        vDiffuse.rgb = g_tUiInfo.vColor.rgb;
    }
	
	
    if (g_tUiInfo.bMask)
    {
        vector vMask =  g_Textures[g_iMask].Sample(g_LinearSampler, In.vTexcoord);
		
        if (0.0f == vMask.a)
            discard;

        if (0.0f == vDiffuse.a)
        {
            vDiffuse = vMask;
        }
    }
    else
    {
        if (0.f == vDiffuse.a)
            discard;
    }

	// 현재 픽셀에서 중점 픽셀을 뺀 값을 넣어줍니다. // UV는 0~1 이므로 중점은 (0.5, 0.5)입니다. // 라디안 값
    float RadianAngle = atan2(In.vTexcoord.y - 0.5f, In.vTexcoord.x - 0.5f);

	// 라디안 각도를 디그리 각도로 바꿔줍니다. (원할한 계산을 위해)
    float DegreeAngle = degrees(RadianAngle);
	
	//구한 디그리 각도는 -180도 ~ 180도 기준이므로 0도 ~ 360도로 변환해줍니다.
    DegreeAngle = DegreeAngle + 180.0f;
	
    if (DegreeAngle >= 0.0f && DegreeAngle < 90.0f)
    {
        DegreeAngle = DegreeAngle + 270.f;
    }
    else if (DegreeAngle >= 90.0f && DegreeAngle <= 360.f)
    {
        DegreeAngle = DegreeAngle - 90.f;
    }
	
	// 내가 설정한 비율에 따른 각도를 구해줍니다.
    float fRatioAngle = g_tUiInfo.fRatio * 360.f;
	
	// 구한 픽셀의 각도와 내가 설정한 비율의 각도를 비교합니다.
    if (DegreeAngle > fRatioAngle)
    {
        discard;
    }
	
    Out.vColor = vDiffuse;
	

    if (g_tUiInfo.bAlpha)
    {
        Out.vColor.a = vDiffuse.a * g_tUiInfo.fAlpha;
    }

    return Out;
}

PS_OUT PS_MAIN_LINEARHORIZONTALPROGRESS(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0.0f;
	
    vector vDiffuse =  g_Textures[g_iBase].Sample(g_LinearSampler, In.vTexcoord);
	
    if (g_tUiInfo.bColor)
    {
        vDiffuse.rgb = g_tUiInfo.vColor.rgb;
    }
	
    if (g_tUiInfo.bMask)
    {
        vector vMask =  g_Textures[g_iMask].Sample(g_LinearSampler, In.vTexcoord);
		
        if (0.0f == vMask.a)
            discard;

        if (0.0f == vDiffuse.a)
        {
            vDiffuse = vMask;
        }
    }
    else
    {
        if (0.f == vDiffuse.a)
            discard;
    }
	
	//if(0.f == Out.vColor.a)
	//	discard;
	
    if (In.vTexcoord.x > g_tUiInfo.fRatio)
    {
        discard;
    }
	
    Out.vColor = vDiffuse;

    if (g_tUiInfo.bAlpha)
    {
        Out.vColor.a = vDiffuse.a * g_tUiInfo.fAlpha;
    }
 
    return Out;
}

PS_OUT PS_MAIN_LINEARVERTICALPROGRESS(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0.0f;
	
    vector vDiffuse =  g_Textures[g_iBase].Sample(g_LinearSampler, In.vTexcoord);
	
    if (g_tUiInfo.bColor)
    {
        vDiffuse.rgb = g_tUiInfo.vColor.rgb;
    }
	
    if (g_tUiInfo.bMask)
    {
        vector vMask =  g_Textures[g_iMask].Sample(g_LinearSampler, In.vTexcoord);
		
        if (0.0f == vMask.a)
            discard;

        if (0.0f == vDiffuse.a)
        {
            vDiffuse = vMask;
        }
    }
    else
    {
        if (0.f == vDiffuse.a)
            discard;
    }

    if ((1.0f - In.vTexcoord.y) > g_tUiInfo.fRatio)
    {
        discard;
    }
	
    Out.vColor = vDiffuse;
	
    if (g_tUiInfo.bAlpha)
    {
        Out.vColor.a = vDiffuse.a * g_tUiInfo.fAlpha;
    }

    return Out;
}

PS_OUT PS_MAIN_IMAGEANIMATION(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0.0f;
	
    vector vDiffuse =  g_Textures[g_iBase].Sample(g_LinearSampler, In.vTexcoord);
	
	//if (0.f == Out.vColor.a)
	//	discard;
	
    if (g_tUiInfo.bColor)
    {
        vDiffuse.rgb = g_tUiInfo.vColor.rgb;
    }
	
    if (g_tUiInfo.bMask)
    {
        vector vMask =  g_Textures[g_iMask].Sample(g_LinearSampler, In.vTexcoord);
		
        if (0.0f == vMask.a)
            discard;
        else if (vDiffuse.a != 1.0f)
        {
            vDiffuse += vMask;
        }
        
        //if (0.0f == vDiffuse.a)
        //    discard;
        
        //if (0.0f == vDiffuse.a)
        //{
        //    vDiffuse = vMask;
        //}
    }
    else
    {
        if (0.f == vDiffuse.a)
            discard;
    }
	
    Out.vColor = vDiffuse;
	
    Out.vColor.a = vDiffuse.a * g_tUiInfo.fAlpha;
	
    return Out;
}

PS_OUT PS_MAIN_LINEARINTERPOLATIONPROGRESS(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0.0f;
	
    vector vDiffuse =  g_Textures[g_iBase].Sample(g_LinearSampler, In.vTexcoord);
	
    if (g_tUiInfo.bColor)
    {
        vDiffuse.rgb = g_tUiInfo.vColor.rgb;
    }
	
    if (In.vTexcoord.x > g_tUiInfo_ProgressBar.fOtherRatio)
    {
        vDiffuse.rgb = g_tUiInfo_ProgressBar.vOtherColor.rgb;
    }
	
    if (In.vTexcoord.x > g_tUiInfo.fRatio)
    {
        discard;
    }
	
    Out.vColor = vDiffuse;
	
    //if (g_bAlpha)
    //    Out.vColor.a = g_fAlpha;
	
    if (g_tUiInfo.bAlpha)
    {
        Out.vColor.a = vDiffuse.a * g_tUiInfo.fAlpha;
    }
	
    return Out;
}

PS_OUT PS_MAIN_MASKIMAGE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0.0f;

    vector vDiffuse = g_Textures[g_iBase].Sample(g_LinearSampler, In.vTexcoord);
	
    if (g_tUiInfo.bColor)
    {
        vDiffuse.rgb = g_tUiInfo.vColor.rgb;
    }
	
    if (g_tUiInfo.bMask)
    {
        vector vMask = g_Textures[g_iMask].Sample(g_LinearSampler, In.vTexcoord);

        float fAlpha = 1.0f - (1.0f - vMask.a) * (1.0f - vDiffuse.a);
        
        float3 vBlendColor = (vMask.rgb * vMask.a + vDiffuse.rgb * vDiffuse.a * (1.0f - vMask.a)) / fAlpha;
        
        vDiffuse = float4(vBlendColor, fAlpha);
    }
    else
    {
        if (0.f == vDiffuse.a)
            discard;
    }
	
    Out.vColor = vDiffuse;
	
    if (g_tUiInfo.bAlpha)
    {
        Out.vColor.a = vDiffuse.a * g_tUiInfo.fAlpha;
    }
	
    return Out;
}

PS_OUT PS_MAIN_POINTIMAGEANIMATION(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0.0f;
	
    vector vDiffuse = g_Textures[g_iBase].Sample(g_PointSampler, In.vTexcoord);
	
    if (g_tUiInfo.bColor)
    {
        vDiffuse.rgb = g_tUiInfo.vColor.rgb;
    }
	
    if (g_tUiInfo.bMask)
    {
        vector vMask = g_Textures[g_iMask].Sample(g_LinearSampler, In.vTexcoord);
		
        if (0.0f == vMask.a)
            discard;
        else if (vDiffuse.a != 1.0f)
        {
            vDiffuse += vMask;
        }
    }
    else
    {
        if (0.f == vDiffuse.a)
            discard;
    }
	
    Out.vColor = vDiffuse;
	
    Out.vColor.a = vDiffuse.a * g_tUiInfo.fAlpha;
	//Out.vColor.a = g_fAlpha;
	
    return Out;
}

PS_OUT PS_MAIN_CIRCLEINTERPOLATIONPROGRESS(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0.0f;
	
    vector vDiffuse = g_Textures[g_iBase].Sample(g_LinearSampler, In.vTexcoord);
	
    if (g_tUiInfo.bColor)
    {
        vDiffuse.rgb = g_tUiInfo.vColor.rgb;
    }
	
 	// 현재 픽셀에서 중점 픽셀을 뺀 값을 넣어줍니다. // UV는 0~1 이므로 중점은 (0.5, 0.5)입니다. // 라디안 값
    float RadianAngle = atan2(In.vTexcoord.y - 0.5f, In.vTexcoord.x - 0.5f);

	// 라디안 각도를 디그리 각도로 바꿔줍니다. (원할한 계산을 위해)
    float DegreeAngle = degrees(RadianAngle);
	
	//구한 디그리 각도는 -180도 ~ 180도 기준이므로 0도 ~ 360도로 변환해줍니다.
    DegreeAngle = DegreeAngle + 180.0f;
	
    if (DegreeAngle >= 0.0f && DegreeAngle < 90.0f)
    {
        DegreeAngle = DegreeAngle + 270.f;
    }
    else if (DegreeAngle >= 90.0f && DegreeAngle <= 360.f)
    {
        DegreeAngle = DegreeAngle - 90.f;
    }
	
	// 내가 설정한 비율에 따른 각도를 구해줍니다.
    float fRatioAngle = g_tUiInfo.fRatio * 360.f;
	
    float fOtherRatioAngle = g_tUiInfo_ProgressBar.fOtherRatio * 360.0f;
    
    if (DegreeAngle < (360.0f - fOtherRatioAngle))
    {
        vDiffuse.rgb = g_tUiInfo_ProgressBar.vOtherColor.rgb;
    }
    
	// 구한 픽셀의 각도와 내가 설정한 비율의 각도를 비교합니다.
    if (DegreeAngle < (360.0f - fRatioAngle))
    {
        discard;
    }
	
    Out.vColor = vDiffuse;

    if (g_tUiInfo.bAlpha)
    {
        Out.vColor.a = vDiffuse.a * g_tUiInfo.fAlpha;
    }

    return Out;
}

PS_OUT PS_MAIN_REDIAMGE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0.0f;

    vector vDiffuse = g_Textures[g_iBase].Sample(g_LinearSampler, In.vTexcoord);
	
    if (g_tUiInfo.bColor)
    {
        vDiffuse.rgb = g_tUiInfo.vColor.rgb;
    }
	
    if (g_tUiInfo.bMask)
    {
        vector vMask = g_Textures[g_iMask].Sample(g_LinearSampler, In.vTexcoord);
		
        if (0.0f == vMask.a)
            discard;

        if (0.0f == vDiffuse.a)
            discard;
    }
    else
    {
        if (0.f == vDiffuse.r)
            discard;
    }
	
    Out.vColor = vDiffuse;
	
    if (g_tUiInfo.bAlpha)
    {
        Out.vColor.a = vDiffuse.r * g_tUiInfo.fAlpha;
    }
	
    return Out;
}

PS_OUT PS_MAIN_STATICPOINT(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0.0f;

    vector vDiffuse = g_Textures[g_iBase].Sample(g_PointSampler, In.vTexcoord);
	
    if (g_tUiInfo.bColor)
    {
        vDiffuse.rgb = g_tUiInfo.vColor.rgb;
    }
	
    if (g_tUiInfo.bMask)
    {
        vector vMask = g_Textures[g_iMask].Sample(g_PointSampler, In.vTexcoord);
		
        if (0.0f == vMask.a)
            discard;

        if (0.0f == vDiffuse.a)
            discard;
    }
    else
    {
        if (0.f == vDiffuse.a)
            discard;
    }
	
    Out.vColor = vDiffuse;
	
    if (g_tUiInfo.bAlpha)
    {
        Out.vColor.a = vDiffuse.a * g_tUiInfo.fAlpha;
    }
	
    return Out;
}

/* EffectFramework */
technique11 DefaultTechnique
{
    pass StaticPass
    {
		/* RenderState설정. */
        SetRasterizerState(RS_None_Cull);
        SetDepthStencilState(DSS_None_ZTestAndWrite, 0);
        SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN_STATIC();
    }

    pass SliderPass
    {
		/* RenderState설정. */
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_ZTestAndWrite, 0);
        SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN_SLIDER();
    }

    pass CircleProgressPass
    {
        SetRasterizerState(RS_None_Cull);
        SetDepthStencilState(DSS_None_ZTestAndWrite, 0);
        SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN_CIRCLEPROGRESS();
    }

    pass LinearHorizontalProgressPass
    {
        SetRasterizerState(RS_None_Cull);
        SetDepthStencilState(DSS_None_ZTestAndWrite, 0);
        SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN_LINEARHORIZONTALPROGRESS();
    }

    pass LinearVerticalProgressPass
    {
        SetRasterizerState(RS_None_Cull);
        SetDepthStencilState(DSS_None_ZTestAndWrite, 0);
        SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN_LINEARVERTICALPROGRESS();
    }

    pass ImageAnimationPass
    {
        SetRasterizerState(RS_None_Cull);
        SetDepthStencilState(DSS_None_ZTestAndWrite, 0);
        SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN_IMAGEANIMATION();
    }

    pass LinearInterpolationProgressPass
    {
        SetRasterizerState(RS_None_Cull);
        SetDepthStencilState(DSS_None_ZTestAndWrite, 0);
        SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN_LINEARINTERPOLATIONPROGRESS();
    }

    pass FrameAnimationPass
    {
        SetRasterizerState(RS_None_Cull);
        SetDepthStencilState(DSS_None_ZTestAndWrite, 0);
        SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_FRAMEANIMATION_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN_IMAGEANIMATION();
    }

    pass MaskImagePass
    {
        SetRasterizerState(RS_None_Cull);
        SetDepthStencilState(DSS_None_ZTestAndWrite, 0);
        SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN_MASKIMAGE();
    }

    pass PointImageAnimationPass
    {
        SetRasterizerState(RS_None_Cull);
        SetDepthStencilState(DSS_None_ZTestAndWrite, 0);
        SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN_POINTIMAGEANIMATION();
    }

    pass CircleInterpolationProgressPass
    {
        SetRasterizerState(RS_None_Cull);
        SetDepthStencilState(DSS_None_ZTestAndWrite, 0);
        SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN_CIRCLEINTERPOLATIONPROGRESS();
    }   

    pass RedImagePass
    {
		/* RenderState설정. */
        SetRasterizerState(RS_None_Cull);
        SetDepthStencilState(DSS_None_ZTestAndWrite, 0);
        SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN_REDIAMGE();
    }

    pass PointImagePass
    {
		/* RenderState설정. */
        SetRasterizerState(RS_None_Cull);
        SetDepthStencilState(DSS_None_ZTestAndWrite, 0);
        SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN_STATICPOINT();
    }
}