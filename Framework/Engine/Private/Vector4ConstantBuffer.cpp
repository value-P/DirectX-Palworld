#include "pch.h"
#include "Vector4ConstantBuffer.h"

#include "DeviceManager.h"

HRESULT Engine::CVector4ConstantBuffer::Initialize(int32 _iRegister, EConstantBufferType _eConstantBufferType)
{
	if (FAILED(CConstantBuffer::Initialize(_iRegister, _eConstantBufferType))) { return E_FAIL; }

	switch (_eConstantBufferType)
	{
	case Engine::EConstantBufferType::ONE_VECTOR4:
		m_spOneVector4ConstantBuffer = make_shared<FOneVector4ConstantBuffer>();
		break;

	case Engine::EConstantBufferType::TWO_VECTOR4:
		m_spTwoVector4ConstantBuffer = make_shared<FTwoVector4ConstantBuffer>();
		break;

	case Engine::EConstantBufferType::THREE_VECTOR4:
		m_spThreeVector4ConstantBuffer = make_shared<FThreeVector4ConstantBuffer>();
		break;

	case Engine::EConstantBufferType::FOUR_VECTOR4:
		m_spFourVector4ConstantBuffer = make_shared<FFourVector4ConstantBuffer>();
		break;

	default:
		CHECKF(false, L"Constant buffer type is not same with its class");
		break;
	}

	return S_OK;
}

void Engine::CVector4ConstantBuffer::Release()
{
	m_spOneVector4ConstantBuffer = nullptr;
	m_spTwoVector4ConstantBuffer = nullptr;
	m_spThreeVector4ConstantBuffer = nullptr;
	m_spFourVector4ConstantBuffer = nullptr;
	CConstantBuffer::Release();
}

void Engine::CVector4ConstantBuffer::TickConstantBuffer(const std::shared_ptr<FConstantBuffer>& _spConstantBuffer)
{
	switch (m_eConstantBufferType)
	{
	case Engine::EConstantBufferType::ONE_VECTOR4:
	{
		shared_ptr<FOneVector4ConstantBuffer> spOneVector4ConstantBuffer = static_pointer_cast<FOneVector4ConstantBuffer>(_spConstantBuffer);
		m_spOneVector4ConstantBuffer->m_vVector4_0 = spOneVector4ConstantBuffer->m_vVector4_0;
	}
	break;

	case Engine::EConstantBufferType::TWO_VECTOR4:
	{
		shared_ptr<FTwoVector4ConstantBuffer> spTwoVector4ConstantBuffer = static_pointer_cast<FTwoVector4ConstantBuffer>(_spConstantBuffer);
		m_spTwoVector4ConstantBuffer->m_vVector4_0 = spTwoVector4ConstantBuffer->m_vVector4_0;
		m_spTwoVector4ConstantBuffer->m_vVector4_1 = spTwoVector4ConstantBuffer->m_vVector4_1;
	}
	break;

	case Engine::EConstantBufferType::THREE_VECTOR4:
	{
		shared_ptr<FThreeVector4ConstantBuffer> spThreeVector4ConstantBuffer = static_pointer_cast<FThreeVector4ConstantBuffer>(_spConstantBuffer);
		m_spThreeVector4ConstantBuffer->m_vVector4_0 = spThreeVector4ConstantBuffer->m_vVector4_0;
		m_spThreeVector4ConstantBuffer->m_vVector4_1 = spThreeVector4ConstantBuffer->m_vVector4_1;
		m_spThreeVector4ConstantBuffer->m_vVector4_2 = spThreeVector4ConstantBuffer->m_vVector4_2;
	}
	break;

	case Engine::EConstantBufferType::FOUR_VECTOR4:
	{
		shared_ptr<FFourVector4ConstantBuffer> spFourVector4ConstantBuffer = static_pointer_cast<FFourVector4ConstantBuffer>(_spConstantBuffer);
		m_spFourVector4ConstantBuffer->m_vVector4_0 = spFourVector4ConstantBuffer->m_vVector4_0;
		m_spFourVector4ConstantBuffer->m_vVector4_1 = spFourVector4ConstantBuffer->m_vVector4_1;
		m_spFourVector4ConstantBuffer->m_vVector4_2 = spFourVector4ConstantBuffer->m_vVector4_2;
		m_spFourVector4ConstantBuffer->m_vVector4_3 = spFourVector4ConstantBuffer->m_vVector4_3;
	}
	break;

	default:
		CHECKF(false, L"Constant buffer type is not same with its class");
		break;
	}

	m_bDirty = true;
}

void Engine::CVector4ConstantBuffer::CopyConstantBuffer()
{
	if (!m_bDirty) { return; }
	m_bDirty = false;

	D3D11_MAPPED_SUBRESOURCE tMappedSubresource;
	m_wpDeviceManager.lock()->GetDeviceContext()->Map(m_cpConstantBuffer.Get(), NULL, D3D11_MAP_WRITE_DISCARD, NULL, &tMappedSubresource);

	switch (m_eConstantBufferType)
	{
	case Engine::EConstantBufferType::ONE_VECTOR4:
		memcpy(tMappedSubresource.pData, m_spOneVector4ConstantBuffer.get(), sizeof(FOneVector4ConstantBuffer));
		break;

	case Engine::EConstantBufferType::TWO_VECTOR4:
		memcpy(tMappedSubresource.pData, m_spTwoVector4ConstantBuffer.get(), sizeof(FTwoVector4ConstantBuffer));
		break;

	case Engine::EConstantBufferType::THREE_VECTOR4:
		memcpy(tMappedSubresource.pData, m_spThreeVector4ConstantBuffer.get(), sizeof(FThreeVector4ConstantBuffer));
		break;

	case Engine::EConstantBufferType::FOUR_VECTOR4:
		memcpy(tMappedSubresource.pData, m_spFourVector4ConstantBuffer.get(), sizeof(FFourVector4ConstantBuffer));
		break;

	default:
		CHECKF(false, L"Constant buffer type is not same with its class");
		break;
	}

	m_wpDeviceManager.lock()->GetDeviceContext()->Unmap(m_cpConstantBuffer.Get(), NULL);
}
