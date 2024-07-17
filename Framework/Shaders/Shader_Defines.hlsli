#ifndef PI
#define PI 3.14159265f
#endif

sampler g_LinearSampler = sampler_state
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = WRAP;
    AddressV = WRAP;
};

sampler g_PointSampler = sampler_state
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = WRAP;
    AddressV = WRAP;
};

/* For.Render_State */
sampler g_ClampSampler = sampler_state
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = CLAMP;
    AddressV = CLAMP;
    AddressW = CLAMP;
};

/* Rasterizer State : D3D11_RASTERIZER_DESC */
RasterizerState RS_Default
{
    FillMode = Solid;
    CullMode = Back;
    FrontCounterClockwise = false;
};

RasterizerState RS_Reverse
{
    FillMode = Solid;
    CullMode = None;
    FrontCounterClockwise = true;
};

RasterizerState RS_None_Cull
{
    FillMode = Solid;
    CullMode = None;
    FrontCounterClockwise = false;
};

RasterizerState RS_Wireframe
{
    FillMode = WireFrame;
    FrontCounterClockwise = false;
};

/* DepthStencil State : D3D11_DEPTH_STENCIL_DESC */
DepthStencilState DSS_Default
{
    DepthEnable = true;
    DepthWriteMask = all;
    DepthFunc = greater;
};

DepthStencilState DSS_OnlyZTest
{
    DepthEnable = true;
    DepthWriteMask = zero;
    DepthFunc = greater;
};

DepthStencilState DSS_None_ZTestAndWrite
{
    DepthEnable = false;
    DepthWriteMask = zero;
};

/* Blend State : D3D11_RENDER_TARGET_BLEND_DESC */
BlendState BS_Default
{
    BlendEnable[0] = false;
};

BlendState BS_AlphaBlending
{
    BlendEnable[0] = true;
    BlendEnable[1] = true;

    SrcBlend = Src_Alpha;
    DestBlend = Inv_Src_Alpha;
    BlendOp = Add;
};

BlendState BS_AlphaBlending_Reverse
{
    BlendEnable[0] = true;
    BlendEnable[1] = true;

    SrcBlend = Inv_Src_Alpha;
    DestBlend = Src_Alpha;
    BlendOp = Add;
};

BlendState BS_Blending
{
    BlendEnable[0] = true;
    BlendEnable[1] = true;

    SrcBlend = one;
    DestBlend = one;
    BlendOp = Add;
};

// 프레임 단위 갱신
struct FFrameInfo
{
    float fDeltaTime;
    float fTotalTime;
    int iDebugRenderTarget;
    int iFramePad1;
};

// 카메라 단위 갱신 
struct FCameraInfo
{
    Matrix matView;

    Matrix matProj;

    Matrix matViewProj;

    float3 vCameraPosition;
    int iCameraPad;
};

FFrameInfo g_tFrame;
FCameraInfo g_tCamera;

float2 nonZeroSign(float2 v)
{
    return float2(v.x >= 0.0 ? 1.0 : -1.0, v.y >= 0.0 ? 1.0 : -1.0);
}