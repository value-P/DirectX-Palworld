#include "pch.h"
#include "ConstantBuffer.h"

#include "EngineModule.h"
#include "DeviceManager.h"

HRESULT Engine::CConstantBuffer::Initialize(int32 _iRegister, EConstantBufferType _eConstantBufferType)
{
	m_wpDeviceManager = CEngineModule::GetInstance()->GetDeviceManager();
	m_eBufferType = EBufferType::CONSTANT;
	m_eConstantBufferType = _eConstantBufferType;
	m_iRegister = _iRegister;

	CHECKF(_eConstantBufferType != EConstantBufferType::ENUM_END, L"Constant buffer type is not designated.");
	CHECKF(_iRegister != -1, L"Register number is not designated.");
	return S_OK;
}

void Engine::CConstantBuffer::Release()
{
	m_cpConstantBuffer = nullptr;
}

void Engine::CConstantBuffer::VSSetConstantBuffers()
{
	if (!m_cpConstantBuffer || m_iRegister < 0) { return; }
	m_wpDeviceManager.lock()->GetDeviceContext()->VSSetConstantBuffers(m_iRegister, 1, m_cpConstantBuffer.GetAddressOf());
}

void Engine::CConstantBuffer::PSSetConstantBuffers()
{
	if (!m_cpConstantBuffer || m_iRegister < 0) { return; }
	m_wpDeviceManager.lock()->GetDeviceContext()->PSSetConstantBuffers(m_iRegister, 1, m_cpConstantBuffer.GetAddressOf());
}

void Engine::CConstantBuffer::GSSetConstantBuffers()
{
	if (!m_cpConstantBuffer || m_iRegister < 0) { return; }
	m_wpDeviceManager.lock()->GetDeviceContext()->GSSetConstantBuffers(m_iRegister, 1, m_cpConstantBuffer.GetAddressOf());
}
