#include "pch.h"
#include "EffectShader.h"

#include "EngineModule.h"
#include "DeviceManager.h"

std::shared_ptr<CEffectShader> Engine::CEffectShader::Create(const ComPtr<ID3DX11Effect>& _cpEffect, const std::vector<ComPtr<ID3D11InputLayout>>& _vecInputLayouts, const ComPtr<ID3D11Device>& _cpDevice, const ComPtr<ID3D11DeviceContext>& _cpDeviceContext)
{
	const std::shared_ptr<CEffectShader> spEffectShader = make_shared<CEffectShader>();
	spEffectShader->Initialize(_cpEffect, _vecInputLayouts, _cpDevice, _cpDeviceContext);
	return spEffectShader;
}

HRESULT Engine::CEffectShader::Initialize(const ComPtr<ID3DX11Effect>& _cpEffect, const std::vector<ComPtr<ID3D11InputLayout>>& _vecInputLayouts, const ComPtr<ID3D11Device>& _cpDevice, const ComPtr<ID3D11DeviceContext>& _cpDeviceContext)
{
	m_cpDevice = _cpDevice;
	m_cpDeviceContext = _cpDeviceContext;

	m_cpEffect = _cpEffect;
	m_vecInputLayouts = _vecInputLayouts;
	return S_OK;
}

void Engine::CEffectShader::Release()
{
	for (auto& cpInputLayout : m_vecInputLayouts)
	{
		cpInputLayout = nullptr;
	}
	m_vecInputLayouts.clear();

	m_cpEffect = nullptr;
}

HRESULT Engine::CEffectShader::BeginPass(uint32 _iPassIndex, uint32 _iTechniqueIndex)
{
	ComPtr<ID3DX11EffectTechnique> cpTechnique = m_cpEffect->GetTechniqueByIndex(_iTechniqueIndex);
	CHECKF(cpTechnique != nullptr, L"Technique is nullptr.");

	ComPtr<ID3DX11EffectPass> cpPass = cpTechnique->GetPassByIndex(_iPassIndex);
	CHECKF(cpPass != nullptr, L"Pass is nullptr.");

	cpPass->Apply(0, m_cpDeviceContext.Get());
	m_cpDeviceContext->IASetInputLayout(m_vecInputLayouts[_iPassIndex].Get());
	return S_OK;
}

ComPtr<ID3DX11EffectVariable> Engine::CEffectShader::GetRawVariableByName(const std::string& _strConstantName)
{
	ComPtr<ID3DX11EffectVariable> cpVariable = m_cpEffect->GetVariableByName(_strConstantName.c_str());
	CHECKF(cpVariable != nullptr, L"Variable is nullptr.");

	return cpVariable;
}

ComPtr<ID3DX11EffectMatrixVariable> Engine::CEffectShader::GetMatrixVariableByName(const std::string& _strConstantName)
{
	ComPtr<ID3DX11EffectVariable> cpVariable = m_cpEffect->GetVariableByName(_strConstantName.c_str());
	CHECKF(cpVariable != nullptr, L"Variable is nullptr.");

	ComPtr<ID3DX11EffectMatrixVariable> cpMatrixVariable = cpVariable->AsMatrix();
	CHECKF(cpMatrixVariable != nullptr, L"MatrixVariable is nullptr.");

	return cpMatrixVariable;
}

ComPtr<ID3DX11EffectShaderResourceVariable> Engine::CEffectShader::GetShaderResourceVariableByName(const std::string& _strConstantName)
{
	ComPtr<ID3DX11EffectVariable> cpVariable = m_cpEffect->GetVariableByName(_strConstantName.c_str());
	CHECKF(cpVariable != nullptr, L"Variable is nullptr.");

	ComPtr<ID3DX11EffectShaderResourceVariable> cpShaderResourceVariable = cpVariable->AsShaderResource();
	CHECKF(cpShaderResourceVariable != nullptr, L"Shader Resource is nullptr.");

	return cpShaderResourceVariable;
}