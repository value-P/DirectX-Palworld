#include "pch.h"
#include "Frustum.h"

std::shared_ptr<CFrustum> Client::CFrustum::Create()
{
	const std::shared_ptr<CFrustum> spFrustum = make_shared<CFrustum>();
	ENSUREF(spFrustum->Initialize(), L"Failed to initialize frustum.");
	return spFrustum;
}

HRESULT Client::CFrustum::Initialize()
{
	m_arrLocalPoints[0] = { -1.0f,  1.0f, 0.0f };
	m_arrLocalPoints[1] = { 1.0f,  1.0f, 0.0f };
	m_arrLocalPoints[2] = { 1.0f, -1.0f, 0.0f };
	m_arrLocalPoints[3] = { -1.0f, -1.0f, 0.0f };
	m_arrLocalPoints[4] = { -1.0f,  1.0f, 1.0f };
	m_arrLocalPoints[5] = { 1.0f,  1.0f, 1.0f };
	m_arrLocalPoints[6] = { 1.0f, -1.0f, 1.0f };
	m_arrLocalPoints[7] = { -1.0f, -1.0f, 1.0f };
	return S_OK;
}

int32 Client::CFrustum::Tick()
{
	return 0;
}
