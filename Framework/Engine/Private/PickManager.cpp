#include "pch.h"
#include "PickManager.h"

// Module
#include "EngineModule.h"

// Manager
#include "DeviceManager.h"
#include "InputManager.h"
#include "CameraManager.h"
#include "PipelineManager.h"

// View
#include "PipelineView.h"

IMPLEMENT_SINGLETON(CPickManager)

HRESULT Engine::CPickManager::Initialize(HWND _hWnd, int32 _iWinSizeX, int32 _iWinSizeY)
{
	m_hWnd = _hWnd;
	m_iWinSizeX = _iWinSizeX;
	m_iWinSizeY = _iWinSizeY;

	m_wpDeviceManager = CEngineModule::GetInstance()->GetDeviceManager();
	m_wpInputManager = CEngineModule::GetInstance()->GetInputManager();
	m_wpCameraManager = CEngineModule::GetInstance()->GetCameraManager();
	m_wpPipelineManager = CEngineModule::GetInstance()->GetPipelineManager();

	m_cpDevice = m_wpDeviceManager.lock()->GetDevice();
	m_cpDeviceContext = m_wpDeviceManager.lock()->GetDeviceContext();
	m_cpSwapChain = m_wpDeviceManager.lock()->GetSwapChain();

	// 24-03-31 �̵��� �߰�
	{
		// 1x1�ȼ� ũ���� �����͸� �����ϱ� ���� Staging Buffer �Դϴ�.
		D3D11_TEXTURE2D_DESC tCopyTexture2DDesc = {};
		ZeroMemory(&tCopyTexture2DDesc, sizeof(tCopyTexture2DDesc));
		tCopyTexture2DDesc.Width = 1;
		tCopyTexture2DDesc.Height = 1;
		tCopyTexture2DDesc.MipLevels = 1;
		tCopyTexture2DDesc.ArraySize = 1;
		tCopyTexture2DDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		tCopyTexture2DDesc.SampleDesc.Count = 1;
		tCopyTexture2DDesc.SampleDesc.Quality = 0;
		tCopyTexture2DDesc.Usage = D3D11_USAGE_STAGING;
		tCopyTexture2DDesc.BindFlags = 0;
		tCopyTexture2DDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
		tCopyTexture2DDesc.MiscFlags = 0;
		ENSURE(m_cpDevice->CreateTexture2D(&tCopyTexture2DDesc, nullptr, m_cpCopyTexture2D.GetAddressOf()));
	}

	return S_OK;
}

int32 Engine::CPickManager::Tick(float _fDeltaSeconds)
{
	return 0;
}

void Engine::CPickManager::Release()
{
	m_cpSwapChain = nullptr;
	m_cpDeviceContext = nullptr;
	m_cpDevice = nullptr;

	m_spInstance = nullptr;
}

void Engine::CPickManager::PickGameObject()
{
	if (!m_bPickMode) { return; }

	// Ŀ�� ��ġ�� ���� �ڽ��� ����ϴ�.
	pair<int32, int32> prCursor = m_wpInputManager.lock()->GetCursorScreenPosition();

	D3D11_BOX tBox = {};
	{
		tBox.left = prCursor.first;
		tBox.right = prCursor.first + 1;
		tBox.top = prCursor.second;
		tBox.bottom = prCursor.second + 1;
		tBox.front = 0;
		tBox.back = 1;
	}


	/*
	// ���� ������Ʈ ��ŷ ���� Ÿ�� �信�� ���콺�� Ŭ���� ��ġ�� �������� �ؽ�ó�� �����մϴ�.
	m_cpDeviceContext->CopySubresourceRegion(m_cpCopyTexture2D.Get(), 0, 0, 0, 0, , 0, &tBox);

	// Ŭ���� ��ġ�� ����Ǿ� �ִ� ���� ��ǥ�� ���� ������Ʈ �ĺ��ڸ� �����մϴ�.
	float szPickData[4]{ 0 };

	D3D11_MAPPED_SUBRESOURCE tSubResource;
	ZeroMemory(&tSubResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	m_cpDeviceContext->Map(m_cpCopyTexture2D.Get(), 0, D3D11_MAP_READ, 0, &tSubResource);
	memcpy(szPickData, tSubResource.pData, 4 * sizeof(float));
	m_cpDeviceContext->Unmap(m_cpCopyTexture2D.Get(), 0);

	Vector3 vPickPosition = Vector3(szPickData[0], szPickData[1], szPickData[2]);
	uint32 iGameObjectID = static_cast<uint32>(szPickData[3]);
	*/

	// 24-03-31 �̵��� �߰�
	{
		if (prCursor.first	< 0 ||
			prCursor.first	> m_iWinSizeX ||
			prCursor.second < 0 ||
			prCursor.second > m_iWinSizeY)
		{
			m_vPickingPos = Vector3(0.f, 0.f, 0.f);
			//m_iGameObjectID = 0;
			return;
		}

		shared_ptr<CPipelineView> spPipelineView = m_wpPipelineManager.lock()->FindPipelineView(EPipelineViewType::BACKBUFFER);
		if (nullptr == spPipelineView) { return; }

		ComPtr<ID3D11Texture2D> cpTextureBuffer = nullptr;
		spPipelineView->m_vecRenderTargetViews[1]->GetResource(reinterpret_cast<ID3D11Resource**>(cpTextureBuffer.GetAddressOf()));
		if (nullptr == cpTextureBuffer) { return; }

		m_cpDeviceContext->OMSetRenderTargets(0, nullptr, nullptr);
		m_cpDeviceContext->CopySubresourceRegion(m_cpCopyTexture2D.Get(), 0, 0, 0, 0, cpTextureBuffer.Get(), 0, &tBox);

		array<float, 4> arrPickingData = { 0.0f, 0.0f, 0.0f, 0.0f };
		D3D11_MAPPED_SUBRESOURCE tMSubResource = {};
		ZeroMemory(&tMSubResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
		ENSURE(m_cpDeviceContext->Map(m_cpCopyTexture2D.Get(), 0, D3D11_MAP_READ, 0, &tMSubResource));
		{
			memcpy(arrPickingData.data(), tMSubResource.pData, 4 * sizeof(float));
			m_cpDeviceContext->Unmap(m_cpCopyTexture2D.Get(), 0);
		}
		m_vPickingPos = Vector3(arrPickingData[0], arrPickingData[1], arrPickingData[2]);
		m_iGameObjectID = static_cast<uint32>(round(arrPickingData[3]));




		//ComPtr<ID3D11Texture2D> cpTextureBuffer = nullptr;
		//ComPtr<ID3D11ShaderResourceView> pSRV = m_wpPipelineManager.lock()->FindShaderResourceView(L"SRV_WORLDPOSITION");

		//// SRV�� ������ �� ���� �ؽ�ó�� ������ ���
		//pSRV->GetResource(reinterpret_cast<ID3D11Resource**>(cpTextureBuffer.GetAddressOf()));

		//m_cpDeviceContext->CopySubresourceRegion(m_cpCopyTexture2D.Get(), 0, 0, 0, 0, cpTextureBuffer.Get(), 0, &tBox);\

		////m_cpDeviceContext->OMSetRenderTargets(0, nullptr, nullptr);

		//array<float, 4> arrPickingData = { 0.0f, 0.0f, 0.0f, 0.0f };
		//D3D11_MAPPED_SUBRESOURCE tMSubResource = {};
		//ZeroMemory(&tMSubResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
		//ENSURE(m_cpDeviceContext->Map(m_cpCopyTexture2D.Get(), 0, D3D11_MAP_READ, 0, &tMSubResource));
		//{
		//	memcpy(arrPickingData.data(), tMSubResource.pData, 4 * sizeof(float));
		//	m_cpDeviceContext->Unmap(m_cpCopyTexture2D.Get(), 0);
		//}
		//m_vPickingPos = Vector3(arrPickingData[0], arrPickingData[1], arrPickingData[2]);
		//m_iGameObjectID = static_cast<uint32>(round(arrPickingData[3]));
	}
}

void Engine::CPickManager::PickMesh()
{
	// �� ��Ʈ�� ���콺 ��ǥ ���ϱ�
	POINT ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(m_hWnd, &ptMouse);

	// ���� �����̽� ���� ��ǥ�� ���ϱ�
	Vector4 vMousePos;
	vMousePos.x = ptMouse.x / (m_iWinSizeX * 0.5f) - 1.f;
	vMousePos.y = ptMouse.y / (m_iWinSizeY * -0.5f) + 1.f;
	vMousePos.z = 0.f;
	vMousePos.w = 1.f;

	// �� �����̽� ���� ��ǥ ���ϱ�
	Matrix matProj;
	matProj = m_wpCameraManager.lock()->GetCurPerspectiveProjectionMatrix().Invert();
	vMousePos = XMVector3TransformCoord(XMLoadFloat4(&vMousePos), XMLoadFloat4x4(&matProj));

	// ���콺 Ŭ�� �� ��ǥ RayPos, ���� RayDir
	Vector4 vRayPosInLocal = { 0.f, 0.f, 0.f, 1.f };
	Vector4 vRayDirInLocal = { 0.f, 0.f, 0.f, 0.f };
	vRayDirInLocal = vMousePos - vRayPosInLocal;

	// ���� �����̽� ���� ��ǥ ���ϱ�
	Matrix 	matView;
	matView = m_wpCameraManager.lock()->GetCurViewMatrix().Invert();

	// ���콺 ���̿� ������ �������� ����
	Vector4 vRayPosInWorld, vRayDirInWorld;
	vRayPosInWorld = XMVector3TransformCoord(vRayPosInWorld, matView);
	vRayDirInWorld = XMVector3TransformNormal(vRayDirInWorld, XMLoadFloat4x4(&matView));
	XMStoreFloat4(&m_vRayPosInWorld, vRayPosInWorld);
	XMStoreFloat4(&m_vRayDirInWorld, XMVector3Normalize(vRayDirInWorld));
}

void Engine::CPickManager::TransformToLocalSpace(const Matrix& _matWorld)
{
	// ���� �����̽��� �ٲ��ش�.
	Matrix	matWorldInverse;
	XMStoreFloat4x4(&matWorldInverse, _matWorld.Invert());

	Vector4 vRayPos, vRayDir;
	vRayPos = XMVector3TransformCoord(XMLoadFloat4(&m_vRayPosInWorld), XMLoadFloat4x4(&matWorldInverse));
	vRayDir = XMVector3TransformNormal(XMLoadFloat4(&m_vRayDirInWorld), XMLoadFloat4x4(&matWorldInverse));
	XMStoreFloat4(&m_vRayPosInLocal, vRayPos);
	XMStoreFloat4(&m_vRayDirInLocal, XMVector3Normalize(vRayDir));
}

bool Engine::CPickManager::PickingInLocal(const Vector3& _vPointA, const Vector3& _vPointB, const Vector3& _vPointC, Vector4& _vPickPos)
{
	float fDist = 0;

	bool bIsColl = TriangleTests::Intersects(XMLoadFloat4(&m_vRayPosInLocal), XMLoadFloat4(&m_vRayDirInLocal), XMLoadFloat3(&_vPointA), XMLoadFloat3(&_vPointB), XMLoadFloat3(&_vPointC), fDist);
	Vector4 vCenter{ _vPointA.x + 0.5f, _vPointA.y, _vPointA.z - 0.5f ,1.f };

	if (bIsColl == false) { _vPickPos = Vector4(-100.f, -100.f, -100.f, 1.f); }
	else { _vPickPos = vCenter; }

	return bIsColl;
}
