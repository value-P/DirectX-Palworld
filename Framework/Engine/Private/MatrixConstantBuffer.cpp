#include "pch.h"
#include "MatrixConstantBuffer.h"

#include "DeviceManager.h"

HRESULT Engine::CMatrixConstantBuffer::Initialize(int32 _iRegister, EConstantBufferType _eConstantBufferType)
{
	if (FAILED(CConstantBuffer::Initialize(_iRegister, _eConstantBufferType))) { return E_FAIL; }

	switch (_eConstantBufferType)
	{
	case Engine::EConstantBufferType::ONE_MATRIX:
		m_spOneMatrixConstantBuffer = make_shared<FOneMatrixConstantBuffer>();
		break;

	case Engine::EConstantBufferType::TWO_MATRIX:
		m_spTwoMatrixConstantBuffer = make_shared<FTwoMatrixConstantBuffer>();
		break;

	case Engine::EConstantBufferType::THREE_MATRIX:
		m_spThreeMatrixConstantBuffer = make_shared<FThreeMatrixConstantBuffer>();
		break;

	case Engine::EConstantBufferType::FOUR_MATRIX:
		m_spFourMatrixConstantBuffer = make_shared<FFourMatrixConstantBuffer>();
		break;

	default:
		CHECKF(false, L"Constant buffer type is not same with its class");
		break;
	}

	return S_OK;
}

void Engine::CMatrixConstantBuffer::Release()
{
	m_spOneMatrixConstantBuffer = nullptr;
	m_spTwoMatrixConstantBuffer = nullptr;
	m_spThreeMatrixConstantBuffer = nullptr;
	m_spFourMatrixConstantBuffer = nullptr;
	CConstantBuffer::Release();
}

void Engine::CMatrixConstantBuffer::TickConstantBuffer(const std::shared_ptr<FConstantBuffer>& _spConstantBuffer)
{
	switch (m_eConstantBufferType)
	{
	case Engine::EConstantBufferType::ONE_MATRIX:
	{
		shared_ptr<FOneMatrixConstantBuffer> spOneMatrixConstantBuffer = static_pointer_cast<FOneMatrixConstantBuffer>(_spConstantBuffer);
		m_spOneMatrixConstantBuffer->m_matMatrix_0 = spOneMatrixConstantBuffer->m_matMatrix_0;
	}
	break;

	case Engine::EConstantBufferType::TWO_MATRIX:
	{
		shared_ptr<FTwoMatrixConstantBuffer> spTwoVector4ConstantBuffer = static_pointer_cast<FTwoMatrixConstantBuffer>(_spConstantBuffer);
		m_spTwoMatrixConstantBuffer->m_matMatrix_0 = spTwoVector4ConstantBuffer->m_matMatrix_0;
		m_spTwoMatrixConstantBuffer->m_matMatrix_1 = spTwoVector4ConstantBuffer->m_matMatrix_1;
	}
	break;

	case Engine::EConstantBufferType::THREE_MATRIX:
	{
		shared_ptr<FThreeMatrixConstantBuffer> spThreeVector4ConstantBuffer = static_pointer_cast<FThreeMatrixConstantBuffer>(_spConstantBuffer);
		m_spThreeMatrixConstantBuffer->m_matMatrix_0 = spThreeVector4ConstantBuffer->m_matMatrix_0;
		m_spThreeMatrixConstantBuffer->m_matMatrix_1 = spThreeVector4ConstantBuffer->m_matMatrix_1;
		m_spThreeMatrixConstantBuffer->m_matMatrix_2 = spThreeVector4ConstantBuffer->m_matMatrix_2;
	}
	break;

	case Engine::EConstantBufferType::FOUR_MATRIX:
	{
		shared_ptr<FFourMatrixConstantBuffer> spFourMatrixConstantBuffer = static_pointer_cast<FFourMatrixConstantBuffer>(_spConstantBuffer);
		m_spFourMatrixConstantBuffer->m_matMatrix_0 = spFourMatrixConstantBuffer->m_matMatrix_0;
		m_spFourMatrixConstantBuffer->m_matMatrix_1 = spFourMatrixConstantBuffer->m_matMatrix_1;
		m_spFourMatrixConstantBuffer->m_matMatrix_2 = spFourMatrixConstantBuffer->m_matMatrix_2;
		m_spFourMatrixConstantBuffer->m_matMatrix_3 = spFourMatrixConstantBuffer->m_matMatrix_3;
	}
	break;

	default:
		CHECKF(false, L"Constant buffer type is not same with its class");
		break;
	}

	m_bDirty = true;
}

void Engine::CMatrixConstantBuffer::CopyConstantBuffer()
{
	if (!m_bDirty) { return; }
	m_bDirty = false;

	D3D11_MAPPED_SUBRESOURCE tMappedSubresource;
	m_wpDeviceManager.lock()->GetDeviceContext()->Map(m_cpConstantBuffer.Get(), NULL, D3D11_MAP_WRITE_DISCARD, NULL, &tMappedSubresource);

	switch (m_eConstantBufferType)
	{
	case Engine::EConstantBufferType::ONE_MATRIX:
		memcpy(tMappedSubresource.pData, m_spOneMatrixConstantBuffer.get(), sizeof(FOneMatrixConstantBuffer));
		break;

	case Engine::EConstantBufferType::TWO_MATRIX:
		memcpy(tMappedSubresource.pData, m_spTwoMatrixConstantBuffer.get(), sizeof(FTwoMatrixConstantBuffer));
		break;

	case Engine::EConstantBufferType::THREE_MATRIX:
		memcpy(tMappedSubresource.pData, m_spThreeMatrixConstantBuffer.get(), sizeof(FThreeMatrixConstantBuffer));
		break;

	case Engine::EConstantBufferType::FOUR_MATRIX:
		memcpy(tMappedSubresource.pData, m_spFourMatrixConstantBuffer.get(), sizeof(FFourMatrixConstantBuffer));
		break;

	default:
		CHECKF(false, L"Constant buffer type is not same with its class");
		break;
	}

	m_wpDeviceManager.lock()->GetDeviceContext()->Unmap(m_cpConstantBuffer.Get(), NULL);
}
