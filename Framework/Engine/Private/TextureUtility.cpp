#include "pch.h"
#include "TextureUtility.h"

D3D11_TEXTURE2D_DESC Engine::CTextureUtility::GetTextureData(const ComPtr<ID3D11ShaderResourceView>& _cpSRV)
{
	ComPtr<ID3D11Texture2D> cpTexture2D = nullptr;
	ComPtr<ID3D11Resource> cpResource = nullptr;
	
	_cpSRV->GetResource(cpResource.GetAddressOf());
	cpResource->QueryInterface<ID3D11Texture2D>(cpTexture2D.GetAddressOf());

	D3D11_TEXTURE2D_DESC tTextureDesc{};
	cpTexture2D->GetDesc(&tTextureDesc);
	return tTextureDesc;
}
