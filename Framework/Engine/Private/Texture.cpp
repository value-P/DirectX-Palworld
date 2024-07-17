#include "pch.h"
#include "Texture.h"
#include "EngineModule.h"
#include "DeviceManager.h"

shared_ptr<CTexture> Engine::CTexture::Create(const wstring& _wstrTextureFilePath)
{
    shared_ptr<CTexture> pInstance = make_shared<CTexture>();

    CHECKF(pInstance != nullptr, L"CTexture Create Failed");

    pInstance->Initialize(_wstrTextureFilePath);
    return pInstance;
}

void Engine::CTexture::Initialize(const wstring& _wstrTextureFilePath)
{
    ComPtr<ID3D11Device> cpDevice = CEngineModule::GetInstance()->GetDeviceManager()->GetDevice();

    filesystem::path filePath(_wstrTextureFilePath);

    wstring wstrExtension = filePath.extension().wstring();
    
    HRESULT hr;
    if (wstrExtension == L".dds")
        hr = CreateDDSTextureFromFile(cpDevice.Get(), _wstrTextureFilePath.c_str(), nullptr, m_cpSRV.GetAddressOf());
    else
        hr = CreateWICTextureFromFile(cpDevice.Get(), _wstrTextureFilePath.c_str(), nullptr, m_cpSRV.GetAddressOf());

    ENSUREF(hr, L"CTexture Initialize Failed");
}
