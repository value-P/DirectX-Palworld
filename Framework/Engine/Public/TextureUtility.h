#pragma once

namespace Engine
{
	class ENGINE_DLL CTextureUtility final
	{
	public:
		static D3D11_TEXTURE2D_DESC GetTextureData(const ComPtr<ID3D11ShaderResourceView>& _cpSRV);
	};
}


