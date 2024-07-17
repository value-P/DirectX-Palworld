#include "pch.h"
#include "TextureHandler.h"

#include "EngineModule.h"
#include "DeviceManager.h"
#include "ThreadManager.h"

IMPLEMENT_SINGLETON(CTextureHandler)

HRESULT Engine::CTextureHandler::Initialize()
{
	shared_ptr<CDeviceManager> spDeviceManager = CEngineModule::GetInstance()->GetDeviceManager();
	m_cpDevice = spDeviceManager->GetDevice();
	m_cpDeviceContext = spDeviceManager->GetDeviceContext();
	m_cpDeferredDeviceContext = spDeviceManager->GetDeferredDeviceContext();
	m_cpMultithread = spDeviceManager->GetMultithread();

	m_wpThreadManager = CEngineModule::GetInstance()->GetThreadManager();
	return S_OK;
}

void Engine::CTextureHandler::Release()
{
	for (auto& pair : m_mapUITextures) { pair.second = nullptr; }
	m_mapUITextures.clear();

	for (auto& pair : m_mapDiffuseTextures) { pair.second = nullptr; }
	m_mapDiffuseTextures.clear();

	for (auto& pair : m_mapNoiseTextures) { pair.second = nullptr; }
	m_mapNoiseTextures.clear();

	for (auto& pair : m_mapMaskTextures) { pair.second = nullptr; }
	m_mapMaskTextures.clear();

	for (auto& pair : m_mapDistortionTextures) { pair.second = nullptr; }
	m_mapDistortionTextures.clear();

	for (auto& pair : m_mapDissolveTextures) { pair.second = nullptr; }
	m_mapDissolveTextures.clear();

	for (auto& pair : m_mapModelTextures) { pair.second = nullptr; }
	m_mapModelTextures.clear();

	m_cpDevice = nullptr;
	m_cpDeviceContext = nullptr;

	m_spInstance = nullptr;
}

HRESULT Engine::CTextureHandler::LoadShaderResourceViewFromFile(const ETextureType& _eTextureType)
{
	wstring wstrTexturePath = L"";

	switch (_eTextureType)
	{
	case ETextureType::UI:			wstrTexturePath = m_wstrUITexturePath;			break;
	case ETextureType::DIFFUSE:		wstrTexturePath = m_wstrDiffuseTexturePath;		break;
	case ETextureType::NOISE:		wstrTexturePath = m_wstrNoiseTexturePath;		break;
	case ETextureType::MASK:		wstrTexturePath = m_wstrMaskTexturePath;		break;
	case ETextureType::DISTORTION:	wstrTexturePath = m_wstrDistortionTexturePath;	break;
	case ETextureType::DISSOLVE:	wstrTexturePath = m_wstrDissolveTexturePath;	break;
	default: CHECKF(false, L"Failed to LoadShaderResourceViewFromFile, Texture type is not defined at CTextureHandler.") break;
	}

	for (const filesystem::directory_entry& fsEntry : filesystem::recursive_directory_iterator(wstrTexturePath))
	{
		if (!filesystem::is_regular_file(fsEntry)) { continue; }

		ENSUREF(LoadShaderResourceView(fsEntry, _eTextureType), L"Failed to load texture.");
	}

	return S_OK;
}

ComPtr<ID3D11ShaderResourceView> Engine::CTextureHandler::FindShaderResourceView(const std::wstring& _wstrShaderResourceViewName, const ETextureType& _eTextureType)
{
	switch (_eTextureType)
	{
	case ETextureType::UI:			if (!m_mapUITextures.contains(_wstrShaderResourceViewName)) { return nullptr; }					break;
	case ETextureType::DIFFUSE:		if (!m_mapDiffuseTextures.contains(_wstrShaderResourceViewName)) { return nullptr; }			break;
	case ETextureType::NOISE:		if (!m_mapNoiseTextures.contains(_wstrShaderResourceViewName)) { return nullptr; }				break;
	case ETextureType::MASK:		if (!m_mapMaskTextures.contains(_wstrShaderResourceViewName)) { return nullptr; }				break;
	case ETextureType::DISTORTION:	if (!m_mapDistortionTextures.contains(_wstrShaderResourceViewName)) { return nullptr; }			break;
	case ETextureType::DISSOLVE:	if (!m_mapDissolveTextures.contains(_wstrShaderResourceViewName)) { return nullptr; }			break;
	case ETextureType::MODEL:		if (!m_mapModelTextures.contains(_wstrShaderResourceViewName)) { return nullptr; }				break;
	case ETextureType::ENUM_END:	CHECKF(false, L"Failed to FindShaderResourceView, Texture type is ENUM_END. : CTextureHandler"); break;
	default: CHECKF(false, L"Failed to FindShaderResourceView, Texture type is not defined at CTextureHandler.");					break;
	}

	switch (_eTextureType)
	{
	case ETextureType::UI:			return m_mapUITextures[_wstrShaderResourceViewName];
	case ETextureType::DIFFUSE:		return m_mapDiffuseTextures[_wstrShaderResourceViewName];
	case ETextureType::NOISE:		return m_mapNoiseTextures[_wstrShaderResourceViewName];
	case ETextureType::MASK:		return m_mapMaskTextures[_wstrShaderResourceViewName];
	case ETextureType::DISTORTION:	return m_mapDistortionTextures[_wstrShaderResourceViewName];
	case ETextureType::DISSOLVE:	return m_mapDissolveTextures[_wstrShaderResourceViewName];
	case ETextureType::MODEL:		return m_mapModelTextures[_wstrShaderResourceViewName];
	default: return nullptr;
	}
}

HRESULT Engine::CTextureHandler::LoadShaderResourceView(const filesystem::directory_entry& _fsEntry, const ETextureType& _eTextureType)
{
	ComPtr<ID3D11ShaderResourceView> cpShaderResourceView = nullptr;
	wstring wstrTextureFileExtenstion = _fsEntry.path().extension().generic_wstring();

	if (wstrTextureFileExtenstion == L".dds" || wstrTextureFileExtenstion == L".DDS")
	{
		if (FAILED(CreateDDSTextureFromFile(m_cpDevice.Get(), _fsEntry.path().c_str(), nullptr, cpShaderResourceView.GetAddressOf())))
		{
			CHECK(false);
			return E_FAIL;
		}
	}
	else if (wstrTextureFileExtenstion == L".png" || wstrTextureFileExtenstion == L".PNG" || wstrTextureFileExtenstion == L".jpg" || wstrTextureFileExtenstion == L".JPG")
	{
		wstring wstrTryingDDS = _fsEntry.path();
		if (wstrTryingDDS.find(L".png"))
		{
			while (true)
			{
				if (wstrTryingDDS.find(L"/") == wstring::npos)
				{
					break;
				}
				wstrTryingDDS = wstrTryingDDS.substr(wstrTryingDDS.find(L"/") + 1);
			}

			while (true)
			{
				if (wstrTryingDDS.find(L"\\") == wstring::npos)
				{
					break;
				}
				wstrTryingDDS = wstrTryingDDS.substr(wstrTryingDDS.find(L"\\") + 1);
			}

			wstrTryingDDS = L"../../Resource/Models/Materials/DDS/" + wstrTryingDDS.substr(0, wstrTryingDDS.find(L".png")) + L".dds";
			if (SUCCEEDED(CreateDDSTextureFromFile(m_cpDevice.Get(), wstrTryingDDS.c_str(), nullptr, cpShaderResourceView.GetAddressOf())))
			{
				return AddShaderResourceView(_fsEntry.path().stem().generic_wstring(), cpShaderResourceView, _eTextureType);
			}
		}

		if (FAILED(CreateWICTextureFromFileEx(m_cpDevice.Get(), m_cpDeviceContext.Get(), _fsEntry.path().c_str(), 0, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, D3D11_RESOURCE_MISC_GENERATE_MIPS, WIC_LOADER_DEFAULT, nullptr, cpShaderResourceView.GetAddressOf())))
		{
			return E_FAIL;
		}
		//if (FAILED(CreateWICTextureFromFile(m_cpDevice.Get(), m_cpDeviceContext.Get(), _fsEntry.path().c_str(), nullptr, cpShaderResourceView.GetAddressOf())))
		//{
		//	return E_FAIL;
		//}
	}
	// 기타 확장자는 불러오지 않습니다.
	else
	{
		return S_OK;
	}

	wstring wstrTextureFileName = _fsEntry.path().stem().generic_wstring();
	return AddShaderResourceView(wstrTextureFileName, cpShaderResourceView, _eTextureType);
}

HRESULT Engine::CTextureHandler::AddShaderResourceView(const wstring& _wstrShaderResourceViewName, const wstring& _wstrTextureFilePath, const ETextureType& _eTextureType)
{
	scoped_lock slLock(m_mtMutex);

	switch (_eTextureType)
	{
	case ETextureType::UI:			if (m_mapUITextures.contains(_wstrShaderResourceViewName)) { return S_OK; } break;
	case ETextureType::DIFFUSE:		if (m_mapDiffuseTextures.contains(_wstrShaderResourceViewName)) { return S_OK; } break;
	case ETextureType::NOISE:		if (m_mapNoiseTextures.contains(_wstrShaderResourceViewName)) { return S_OK; } break;
	case ETextureType::MASK:		if (m_mapMaskTextures.contains(_wstrShaderResourceViewName)) { return S_OK; } break;
	case ETextureType::DISTORTION:	if (m_mapDistortionTextures.contains(_wstrShaderResourceViewName)) { return S_OK; } break;
	case ETextureType::DISSOLVE:	if (m_mapDissolveTextures.contains(_wstrShaderResourceViewName)) { return S_OK; } break;
	case ETextureType::MODEL:		if (m_mapModelTextures.contains(_wstrShaderResourceViewName)) { return S_OK; } break;
	default:						CHECKF(false, L"Texture type is not designated.");									 break;
	}

	ComPtr<ID3D11ShaderResourceView> cpShaderResourceView = nullptr;
	wstring wstrTextureFileExtenstion = _wstrTextureFilePath.substr(_wstrTextureFilePath.find(L".", lstrlenW(L"../../")));

	if (wstrTextureFileExtenstion == L".dds" || wstrTextureFileExtenstion == L".DDS")
	{
		if (FAILED(CreateDDSTextureFromFile(m_cpDevice.Get(), _wstrTextureFilePath.c_str(), nullptr, cpShaderResourceView.GetAddressOf())))
		{
			return E_FAIL;
		}
	}
	else if (wstrTextureFileExtenstion == L".png" || wstrTextureFileExtenstion == L".PNG" || wstrTextureFileExtenstion == L".jpg" || wstrTextureFileExtenstion == L".JPG")
	{
		wstring wstrTryingDDS = _wstrTextureFilePath;
		if (_wstrTextureFilePath.find(L".png"))
		{
			while (true)
			{
				if (wstrTryingDDS.find(L"/") == wstring::npos)
				{
					break;
				}
				wstrTryingDDS = wstrTryingDDS.substr(wstrTryingDDS.find(L"/") + 1);
			}

			while (true)
			{
				if (wstrTryingDDS.find(L"\\") == wstring::npos)
				{
					break;
				}
				wstrTryingDDS = wstrTryingDDS.substr(wstrTryingDDS.find(L"\\") + 1);
			}

			wstrTryingDDS = L"../../Resource/Models/Materials/DDS/" + wstrTryingDDS.substr(0, wstrTryingDDS.find(L".png")) + L".dds";
			if (SUCCEEDED(CreateDDSTextureFromFile(m_cpDevice.Get(), wstrTryingDDS.c_str(), nullptr, cpShaderResourceView.GetAddressOf())))
			{
				return AddShaderResourceView(_wstrShaderResourceViewName, cpShaderResourceView, _eTextureType);
			}
		}

		if (FAILED(CreateWICTextureFromFileEx(m_cpDevice.Get(), m_cpDeviceContext.Get(), _wstrTextureFilePath.c_str(), 0, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, D3D11_RESOURCE_MISC_GENERATE_MIPS, WIC_LOADER_DEFAULT, nullptr, cpShaderResourceView.GetAddressOf())))
		{
			return E_FAIL;
		}
		/*
		if (FAILED(CreateWICTextureFromFile(m_cpDevice.Get(), m_cpDeviceContext.Get(), _wstrTextureFilePath.c_str(), nullptr, cpShaderResourceView.GetAddressOf())))
		{
			return E_FAIL;
		}
		*/
	}
	else
	{
		return S_OK;
	}

	return AddShaderResourceView(_wstrShaderResourceViewName, cpShaderResourceView, _eTextureType);
}

HRESULT Engine::CTextureHandler::AddShaderResourceView(const std::wstring& _wstrShaderResourceViewName, const ComPtr<ID3D11ShaderResourceView>& _cpShaderResouceView, const ETextureType& _eTextureType)
{
	switch (_eTextureType)
	{
	case ETextureType::UI:			if (m_mapUITextures.contains(_wstrShaderResourceViewName)) { return E_FAIL; }				break;
	case ETextureType::DIFFUSE:		if (m_mapDiffuseTextures.contains(_wstrShaderResourceViewName)) { return E_FAIL; }				break;
	case ETextureType::NOISE:		if (m_mapNoiseTextures.contains(_wstrShaderResourceViewName)) { return E_FAIL; }				break;
	case ETextureType::MASK:		if (m_mapMaskTextures.contains(_wstrShaderResourceViewName)) { return E_FAIL; }				break;
	case ETextureType::DISTORTION:	if (m_mapDistortionTextures.contains(_wstrShaderResourceViewName)) { return E_FAIL; }				break;
	case ETextureType::DISSOLVE:	if (m_mapDissolveTextures.contains(_wstrShaderResourceViewName)) { return E_FAIL; }				break;
	case ETextureType::MODEL:		if (m_mapModelTextures.contains(_wstrShaderResourceViewName)) { return E_FAIL; }				break;
	case ETextureType::ENUM_END:	CHECKF(false, L"Failed to AddShaderResourceView, Texture type is ENUM_END. : CTextureHandler");		break;
	default:						CHECKF(false, L"Failed to AddShaderResourceView, Texture type is not defined at CTextureHandler");	break;
	}

	switch (_eTextureType)
	{
	case ETextureType::UI:			m_mapUITextures.emplace(_wstrShaderResourceViewName, _cpShaderResouceView);			break;
	case ETextureType::DIFFUSE:		m_mapDiffuseTextures.emplace(_wstrShaderResourceViewName, _cpShaderResouceView);	break;
	case ETextureType::NOISE:		m_mapNoiseTextures.emplace(_wstrShaderResourceViewName, _cpShaderResouceView);		break;
	case ETextureType::MASK:		m_mapMaskTextures.emplace(_wstrShaderResourceViewName, _cpShaderResouceView);		break;
	case ETextureType::DISTORTION:	m_mapDistortionTextures.emplace(_wstrShaderResourceViewName, _cpShaderResouceView); break;
	case ETextureType::MODEL:		m_mapModelTextures.emplace(_wstrShaderResourceViewName, _cpShaderResouceView);		break;
	case ETextureType::DISSOLVE:	m_mapDissolveTextures.emplace(_wstrShaderResourceViewName, _cpShaderResouceView);	break;
	default: break;
	}

	return S_OK;
}
