// default
#include "pch.h"
#include "LightManager.h"
#include "EffectShader.h"


HRESULT Engine::CLightManager::Initialize()
{
	return S_OK;
}

void Engine::CLightManager::Tick()
{
	RefreshLightInfos();
}

void Engine::CLightManager::Release()
{
	ClearLight();
}

HRESULT Engine::CLightManager::AddLight(const std::wstring& _wstrLightName, shared_ptr<FDirectionalLight> _spLight, bool _bActive)
{
	if (m_umapDirLights.find(_wstrLightName) != m_umapDirLights.end()) { return E_FAIL; }
	else
	{
		m_umapDirLights[_wstrLightName] = _spLight;

		if (_bActive)
		{
			if (m_spCurrentDirLight != nullptr)
			{
				m_spCurrentDirLight = nullptr;
			}
			m_spCurrentDirLight = _spLight;
		}
	}
	return S_OK;
}

HRESULT Engine::CLightManager::AddLight(const std::wstring& _wstrLightName, shared_ptr<FPointLight> _spLight, bool _bActive)
{
	if (m_umapPointLights.find(_wstrLightName) != m_umapPointLights.end()) { return E_FAIL; }
	else
	{
		m_umapPointLights[_wstrLightName] = _spLight;

		if (_bActive)
		{
			if (m_umapPointLights.size() < 8)
			{
				m_umapPointLights[_wstrLightName] = _spLight;
			}
		}
	}
	return S_OK;
}

HRESULT Engine::CLightManager::RemoveLight(const std::wstring& _wstrLightName, ELightType _eLightType)
{
	switch (_eLightType)
	{
	case ELightType::DIRECTIONAL:
		if (m_umapDirLights.find(_wstrLightName) == m_umapDirLights.end()) { return E_FAIL; }
		else { m_umapDirLights.erase(_wstrLightName); }
		return S_OK;
	case ELightType::POINT:
		if (m_umapPointLights.find(_wstrLightName) == m_umapPointLights.end()) { return E_FAIL; }
		else { m_umapPointLights.erase(_wstrLightName); }
		return S_OK;
	}
	return E_FAIL;
}

HRESULT Engine::CLightManager::SetActiveLight(const std::wstring& _wstrLightName, ELightType _eLightType, bool _bActive)
{
	switch (_eLightType)
	{
	case ELightType::DIRECTIONAL:
		if (m_umapDirLights.find(_wstrLightName) == m_umapDirLights.end()) { return E_FAIL; }
		else
		{
			if (_bActive)
			{
				if (m_spCurrentDirLight != nullptr)
				{
					m_spCurrentDirLight = nullptr;
				}
				m_spCurrentDirLight = m_umapDirLights[_wstrLightName];
			}
			else
			{
				if (m_spCurrentDirLight != nullptr && m_spCurrentDirLight == m_umapDirLights[_wstrLightName])
				{
					m_spCurrentDirLight = nullptr;
				}
			}
		}
		return S_OK;
	case ELightType::POINT:
		if (m_umapPointLights.find(_wstrLightName) == m_umapPointLights.end()) { return E_FAIL; }
		else
		{
			if (_bActive && m_umapCurrentPointLights.size() < 8)
			{
				m_umapCurrentPointLights[_wstrLightName] = m_umapPointLights[_wstrLightName];
			}
			else
			{
				if (m_umapCurrentPointLights.find(_wstrLightName) != m_umapCurrentPointLights.end())
				{
					m_umapCurrentPointLights.erase(_wstrLightName);
				}
			}

		}
		return S_OK;
	}
	return E_FAIL;
}

HRESULT Engine::CLightManager::ClearLight()
{
	for (auto& Pair : m_umapDirLights)
	{
		Pair.second = nullptr;
	}
	m_umapDirLights.clear();

	m_spCurrentDirLight = nullptr;

	for (auto& Pair : m_umapPointLights)
	{
		Pair.second = nullptr;
	}
	m_umapPointLights.clear();

	for (auto& Pair : m_umapCurrentPointLights)
	{
		Pair.second = nullptr;
	}
	m_umapCurrentPointLights.clear();

	return S_OK;
}

void Engine::CLightManager::RefreshLightInfos()
{
	if (m_spCurrentDirLight != nullptr)
	{
		m_tLightInfo.tDirectionalLight = *m_spCurrentDirLight.get();
	}

	m_tLightInfo.iPointLightCount = 0;
	for (auto& tPointLight : m_umapCurrentPointLights)
	{
		m_tLightInfo.tPointLights[m_tLightInfo.iPointLightCount++] = *tPointLight.second.get();

		if (m_tLightInfo.iPointLightCount <= 8)
		{
			break;
		}
	}

	m_tLightInfo.tAmbientLight = m_tAmbientLight;
}