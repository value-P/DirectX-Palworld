#include "pch.h"
#include "IntConstantBuffer.h"

#include "DeviceManager.h"

HRESULT Engine::CIntConstantBuffer::Initialize(int32 _iRegister, EConstantBufferType _eConstantBufferType)
{
	if (FAILED(CConstantBuffer::Initialize(_iRegister, _eConstantBufferType))) { return E_FAIL; }

	switch (_eConstantBufferType)
	{
	case Engine::EConstantBufferType::ONE_INT:
		m_spOneIntConstantBuffer = make_shared<FOneIntConstantBuffer>();
		break;

	case Engine::EConstantBufferType::TWO_INT:
		m_spTwoIntConstantBuffer = make_shared<FTwoIntConstantBuffer>();
		break;

	case Engine::EConstantBufferType::THREE_INT:
		m_spThreeIntConstantBuffer = make_shared<FThreeIntConstantBuffer>();
		break;

	case Engine::EConstantBufferType::FOUR_INT:
		m_spFourIntConstantBuffer = make_shared<FFourIntConstantBuffer>();
		break;

	default:
		CHECKF(false, L"Constant buffer type is not same with its class");
		break;
	}

	return S_OK;
}

void Engine::CIntConstantBuffer::Release()
{
	m_spOneIntConstantBuffer = nullptr;
	m_spTwoIntConstantBuffer = nullptr;
	m_spThreeIntConstantBuffer = nullptr;
	m_spFourIntConstantBuffer = nullptr;
	CConstantBuffer::Release();
}

void Engine::CIntConstantBuffer::TickConstantBuffer(const std::shared_ptr<FConstantBuffer>& _spConstantBuffer)
{

	switch (m_eConstantBufferType)
	{
	case Engine::EConstantBufferType::ONE_INT:
	{
		shared_ptr<FOneIntConstantBuffer> spOneIntConstantBuffer = static_pointer_cast<FOneIntConstantBuffer>(_spConstantBuffer);
		m_spOneIntConstantBuffer->m_iInt_0 = spOneIntConstantBuffer->m_iInt_0;
	}
	break;

	case Engine::EConstantBufferType::TWO_INT:
	{
		shared_ptr<FTwoIntConstantBuffer> spTwoIntConstantBuffer = static_pointer_cast<FTwoIntConstantBuffer>(_spConstantBuffer);
		m_spTwoIntConstantBuffer->m_iInt_0 = spTwoIntConstantBuffer->m_iInt_0;
		m_spTwoIntConstantBuffer->m_iInt_1 = spTwoIntConstantBuffer->m_iInt_1;
	}
	break;

	case Engine::EConstantBufferType::THREE_INT:
	{
		shared_ptr<FThreeIntConstantBuffer> spThreeIntConstantBuffer = static_pointer_cast<FThreeIntConstantBuffer>(_spConstantBuffer);
		m_spThreeIntConstantBuffer->m_iInt_0 = spThreeIntConstantBuffer->m_iInt_0;
		m_spThreeIntConstantBuffer->m_iInt_1 = spThreeIntConstantBuffer->m_iInt_1;
		m_spThreeIntConstantBuffer->m_iInt_2 = spThreeIntConstantBuffer->m_iInt_2;
	}
	break;

	case Engine::EConstantBufferType::FOUR_INT:
	{
		shared_ptr<FFourIntConstantBuffer> spFourIntConstantBuffer = static_pointer_cast<FFourIntConstantBuffer>(_spConstantBuffer);
		m_spFourIntConstantBuffer->m_iInt_0 = spFourIntConstantBuffer->m_iInt_0;
		m_spFourIntConstantBuffer->m_iInt_1 = spFourIntConstantBuffer->m_iInt_1;
		m_spFourIntConstantBuffer->m_iInt_2 = spFourIntConstantBuffer->m_iInt_2;
		m_spFourIntConstantBuffer->m_iInt_3 = spFourIntConstantBuffer->m_iInt_3;
	}
	break;

	default:
		CHECKF(false, L"Constant buffer type is not same with its class");
		break;
	}

	m_bDirty = true;
}

void Engine::CIntConstantBuffer::CopyConstantBuffer()
{
	if (!m_bDirty) { return; }
	m_bDirty = false;

	D3D11_MAPPED_SUBRESOURCE tMappedSubresource;
	m_wpDeviceManager.lock()->GetDeviceContext()->Map(m_cpConstantBuffer.Get(), NULL, D3D11_MAP_WRITE_DISCARD, NULL, &tMappedSubresource);

	switch (m_eConstantBufferType)
	{
	case Engine::EConstantBufferType::ONE_INT:
		::memcpy(tMappedSubresource.pData, m_spOneIntConstantBuffer.get(), sizeof(FOneIntConstantBuffer));
		break;

	case Engine::EConstantBufferType::TWO_INT:
		::memcpy(tMappedSubresource.pData, m_spTwoIntConstantBuffer.get(), sizeof(FTwoIntConstantBuffer));
		break;

	case Engine::EConstantBufferType::THREE_INT:
		::memcpy(tMappedSubresource.pData, m_spThreeIntConstantBuffer.get(), sizeof(FThreeIntConstantBuffer));
		break;

	case Engine::EConstantBufferType::FOUR_INT:
		::memcpy(tMappedSubresource.pData, m_spFourIntConstantBuffer.get(), sizeof(FFourIntConstantBuffer));
		break;

	default:
		CHECKF(false, L"Constant buffer type is not same with its class");
		break;
	}

	m_wpDeviceManager.lock()->GetDeviceContext()->Unmap(m_cpConstantBuffer.Get(), NULL);
}
