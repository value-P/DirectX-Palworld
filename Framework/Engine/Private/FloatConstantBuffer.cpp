#include "pch.h"
#include "FloatConstantBuffer.h"

#include "DeviceManager.h"

HRESULT Engine::CFloatConstantBuffer::Initialize(int32 _iRegister, EConstantBufferType _eConstantBufferType)
{
	if (FAILED(CConstantBuffer::Initialize(_iRegister, _eConstantBufferType))) { return E_FAIL; }

	switch (_eConstantBufferType)
	{
	case Engine::EConstantBufferType::ONE_FLOAT:
		m_spOneFloatConstantBuffer = make_shared<FOneFloatConstantBuffer>();
		break;

	case Engine::EConstantBufferType::TWO_FLOAT:
		m_spTwoFloatConstantBuffer = make_shared<FTwoFloatConstantBuffer>();
		break;

	case Engine::EConstantBufferType::THREE_FLOAT:
		m_spThreeFloatConstantBuffer = make_shared<FThreeFloatConstantBuffer>();
		break;

	case Engine::EConstantBufferType::FOUR_FLOAT:
		m_spFourFloatConstantBuffer = make_shared<FFourFloatConstantBuffer>();
		break;

	default:
		CHECKF(false, L"Constant buffer type is not same with its class");
		break;
	}

	return S_OK;
}

void Engine::CFloatConstantBuffer::Release()
{	
	m_spOneFloatConstantBuffer = nullptr;
	m_spTwoFloatConstantBuffer = nullptr;
	m_spThreeFloatConstantBuffer = nullptr;
	m_spFourFloatConstantBuffer = nullptr;
	CConstantBuffer::Release();
}

void Engine::CFloatConstantBuffer::TickConstantBuffer(const std::shared_ptr<FConstantBuffer>& _spConstantBuffer)
{
	switch (m_eConstantBufferType)
	{
	case Engine::EConstantBufferType::ONE_FLOAT:
	{
		shared_ptr<FOneFloatConstantBuffer> spOneFloatConstantBuffer = static_pointer_cast<FOneFloatConstantBuffer>(_spConstantBuffer);
		m_spOneFloatConstantBuffer->m_fFloat_0 = spOneFloatConstantBuffer->m_fFloat_0;
	}
	break;

	case Engine::EConstantBufferType::TWO_FLOAT:
	{
		shared_ptr<FTwoFloatConstantBuffer> spTwoFloatConstantBuffer = static_pointer_cast<FTwoFloatConstantBuffer>(_spConstantBuffer);
		m_spTwoFloatConstantBuffer->m_fFloat_0 = spTwoFloatConstantBuffer->m_fFloat_0;
		m_spTwoFloatConstantBuffer->m_fFloat_1 = spTwoFloatConstantBuffer->m_fFloat_1;
	}
	break;

	case Engine::EConstantBufferType::THREE_FLOAT:
	{
		shared_ptr<FThreeFloatConstantBuffer> spThreeFloatConstantBuffer = static_pointer_cast<FThreeFloatConstantBuffer>(_spConstantBuffer);
		m_spThreeFloatConstantBuffer->m_fFloat_0 = spThreeFloatConstantBuffer->m_fFloat_0;
		m_spThreeFloatConstantBuffer->m_fFloat_1 = spThreeFloatConstantBuffer->m_fFloat_1;
		m_spThreeFloatConstantBuffer->m_fFloat_2 = spThreeFloatConstantBuffer->m_fFloat_2;
	}
	break;

	case Engine::EConstantBufferType::FOUR_FLOAT:
	{
		shared_ptr<FFourFloatConstantBuffer> spFourFloatConstantBuffer = static_pointer_cast<FFourFloatConstantBuffer>(_spConstantBuffer);
		m_spFourFloatConstantBuffer->m_fFloat_0 = spFourFloatConstantBuffer->m_fFloat_0;
		m_spFourFloatConstantBuffer->m_fFloat_1 = spFourFloatConstantBuffer->m_fFloat_1;
		m_spFourFloatConstantBuffer->m_fFloat_2 = spFourFloatConstantBuffer->m_fFloat_2;
		m_spFourFloatConstantBuffer->m_fFloat_3 = spFourFloatConstantBuffer->m_fFloat_3;
	}
	break;

	default:
		CHECKF(false, L"Constant buffer type is not same with its class");
		break;
	}

	m_bDirty = true;
}

void Engine::CFloatConstantBuffer::CopyConstantBuffer()
{
	if (!m_bDirty) { return; }
	m_bDirty = false;

	D3D11_MAPPED_SUBRESOURCE tMappedSubresource;
	m_wpDeviceManager.lock()->GetDeviceContext()->Map(m_cpConstantBuffer.Get(), NULL, D3D11_MAP_WRITE_DISCARD, NULL, &tMappedSubresource);

	switch (m_eConstantBufferType)
	{
	case Engine::EConstantBufferType::ONE_FLOAT:
		memcpy(tMappedSubresource.pData, m_spOneFloatConstantBuffer.get(), sizeof(FOneFloatConstantBuffer));
		break;

	case Engine::EConstantBufferType::TWO_FLOAT:
		memcpy(tMappedSubresource.pData, m_spTwoFloatConstantBuffer.get(), sizeof(FTwoFloatConstantBuffer));
		break;

	case Engine::EConstantBufferType::THREE_FLOAT:
		memcpy(tMappedSubresource.pData, m_spThreeFloatConstantBuffer.get(), sizeof(FThreeFloatConstantBuffer));
		break;

	case Engine::EConstantBufferType::FOUR_FLOAT:
		memcpy(tMappedSubresource.pData, m_spFourFloatConstantBuffer.get(), sizeof(FFourFloatConstantBuffer));
		break;

	default:
		CHECKF(false, L"Constant buffer type is not same with its class");
		break;
	}

	m_wpDeviceManager.lock()->GetDeviceContext()->Unmap(m_cpConstantBuffer.Get(), NULL);
}
