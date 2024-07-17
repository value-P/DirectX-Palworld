#include "pch.h"
#include "Vector3ConstantBuffer.h"

#include "DeviceManager.h"

HRESULT Engine::CVector3ConstantBuffer::Initialize(int32 _iRegister, EConstantBufferType _eConstantBufferType)
{
	if (FAILED(CConstantBuffer::Initialize(_iRegister, _eConstantBufferType))) { return E_FAIL; }

	switch (_eConstantBufferType)
	{
	case Engine::EConstantBufferType::ONE_VECTOR3:
		m_spOneVector3ConstantBuffer = make_shared<FOneVector3ConstantBuffer>();
		break;

	case Engine::EConstantBufferType::TWO_VECTOR3:
		m_spTwoVector3ConstantBuffer = make_shared<FTwoVector3ConstantBuffer>();
		break;

	case Engine::EConstantBufferType::THREE_VECTOR3:
		m_spThreeVector3ConstantBuffer = make_shared<FThreeVector3ConstantBuffer>();
		break;

	case Engine::EConstantBufferType::FOUR_VECTOR3:
		m_spFourVector3ConstantBuffer = make_shared<FFourVector3ConstantBuffer>();
		break;

	default:
		CHECKF(false, L"Constant buffer type is not same with its class");
		break;
	}

	return S_OK;
}

void Engine::CVector3ConstantBuffer::Release()
{
	m_spOneVector3ConstantBuffer = nullptr;
	m_spTwoVector3ConstantBuffer = nullptr;
	m_spThreeVector3ConstantBuffer = nullptr;
	m_spFourVector3ConstantBuffer = nullptr;
	CConstantBuffer::Release();
}

void Engine::CVector3ConstantBuffer::TickConstantBuffer(const std::shared_ptr<FConstantBuffer>& _spConstantBuffer)
{
	switch (m_eConstantBufferType)
	{
	case Engine::EConstantBufferType::ONE_VECTOR3:
	{
		shared_ptr<FOneVector3ConstantBuffer> spOneVector3ConstantBuffer = static_pointer_cast<FOneVector3ConstantBuffer>(_spConstantBuffer);
		m_spOneVector3ConstantBuffer->m_vVector3_0 = spOneVector3ConstantBuffer->m_vVector3_0;
	}	
	break;

	case Engine::EConstantBufferType::TWO_VECTOR3:
	{
		shared_ptr<FTwoVector3ConstantBuffer> spTwoVector3ConstantBuffer = static_pointer_cast<FTwoVector3ConstantBuffer>(_spConstantBuffer);
		m_spTwoVector3ConstantBuffer->m_vVector3_0 = spTwoVector3ConstantBuffer->m_vVector3_0;
		m_spTwoVector3ConstantBuffer->m_vVector3_1 = spTwoVector3ConstantBuffer->m_vVector3_1;
	}
	break;

	case Engine::EConstantBufferType::THREE_VECTOR3:
	{
		shared_ptr<FThreeVector3ConstantBuffer> spThreeVector3ConstantBuffer = static_pointer_cast<FThreeVector3ConstantBuffer>(_spConstantBuffer);
		m_spThreeVector3ConstantBuffer->m_vVector3_0 = spThreeVector3ConstantBuffer->m_vVector3_0;
		m_spThreeVector3ConstantBuffer->m_vVector3_1 = spThreeVector3ConstantBuffer->m_vVector3_1;
		m_spThreeVector3ConstantBuffer->m_vVector3_2 = spThreeVector3ConstantBuffer->m_vVector3_2;
	}
	break;

	case Engine::EConstantBufferType::FOUR_VECTOR3:
	{
		shared_ptr<FFourVector3ConstantBuffer> spFourVector3ConstantBuffer = static_pointer_cast<FFourVector3ConstantBuffer>(_spConstantBuffer);
		m_spFourVector3ConstantBuffer->m_vVector3_0 = spFourVector3ConstantBuffer->m_vVector3_0;
		m_spFourVector3ConstantBuffer->m_vVector3_1 = spFourVector3ConstantBuffer->m_vVector3_1;
		m_spFourVector3ConstantBuffer->m_vVector3_2 = spFourVector3ConstantBuffer->m_vVector3_2;
		m_spFourVector3ConstantBuffer->m_vVector3_3 = spFourVector3ConstantBuffer->m_vVector3_3;
	}
	break;

	default:
		CHECKF(false, L"Constant buffer type is not same with its class");
		break;
	}

	m_bDirty = true;
}

void Engine::CVector3ConstantBuffer::CopyConstantBuffer()
{
	if (!m_bDirty) { return; }
	m_bDirty = false;

	D3D11_MAPPED_SUBRESOURCE tMappedSubresource;
	m_wpDeviceManager.lock()->GetDeviceContext()->Map(m_cpConstantBuffer.Get(), NULL, D3D11_MAP_WRITE_DISCARD, NULL, &tMappedSubresource);

	switch (m_eConstantBufferType)
	{
	case Engine::EConstantBufferType::ONE_VECTOR3:
		memcpy(tMappedSubresource.pData, m_spOneVector3ConstantBuffer.get(), sizeof(FOneVector3ConstantBuffer));
		break;

	case Engine::EConstantBufferType::TWO_VECTOR3:
		memcpy(tMappedSubresource.pData, m_spTwoVector3ConstantBuffer.get(), sizeof(FTwoVector3ConstantBuffer));
		break;

	case Engine::EConstantBufferType::THREE_VECTOR3:
		memcpy(tMappedSubresource.pData, m_spThreeVector3ConstantBuffer.get(), sizeof(FThreeVector3ConstantBuffer));
		break;

	case Engine::EConstantBufferType::FOUR_VECTOR3:
		memcpy(tMappedSubresource.pData, m_spFourVector3ConstantBuffer.get(), sizeof(FFourVector3ConstantBuffer));
		break;

	default:
		CHECKF(false, L"Constant buffer type is not same with its class");
		break;
	}

	m_wpDeviceManager.lock()->GetDeviceContext()->Unmap(m_cpConstantBuffer.Get(), NULL);
}
