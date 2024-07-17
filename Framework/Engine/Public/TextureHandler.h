#pragma once

namespace Engine
{
	class CThreadManager;

	class ENGINE_DLL CTextureHandler final
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		DECLARE_SINGLETON(CTextureHandler)

	/********************
		Events
	********************/
	public:
		HRESULT Initialize();

		void Release();

	/********************
		Methods
	********************/
	public:
		HRESULT LoadShaderResourceViewFromFile(const ETextureType& _eTextureType);

		ComPtr<ID3D11ShaderResourceView> FindShaderResourceView(const std::wstring& _wstrShaderResourceViewName, const ETextureType& _eTextureType);
	
	private:
		HRESULT LoadShaderResourceView(const filesystem::directory_entry& _fsEntry, const ETextureType& _eTextureType);

		HRESULT AddShaderResourceView(const std::wstring& _wstrShaderResourceViewName, const ComPtr<ID3D11ShaderResourceView>& _cpShaderResouceView, const ETextureType& _eTextureType);

	public:
		HRESULT AddShaderResourceView(const std::wstring& _wstrShaderResourceViewName, const std::wstring& _wstrTextureFilePath, const ETextureType& _eTextureType);

	/********************
		Getter/Setter
	********************/
	public:
		const std::map<std::wstring, ComPtr<ID3D11ShaderResourceView>>& GetUITextures()         { return m_mapUITextures; }
		const std::map<std::wstring, ComPtr<ID3D11ShaderResourceView>>& GetDiffuseTextures()    { return m_mapDiffuseTextures; }
		const std::map<std::wstring, ComPtr<ID3D11ShaderResourceView>>& GetNoiseTextures()      { return m_mapNoiseTextures; }
		const std::map<std::wstring, ComPtr<ID3D11ShaderResourceView>>& GetMaskTextures()       { return m_mapMaskTextures; }
		const std::map<std::wstring, ComPtr<ID3D11ShaderResourceView>>& GetDistortionTextures() { return m_mapDistortionTextures; }
		const std::map<std::wstring, ComPtr<ID3D11ShaderResourceView>>& GetDissolveTextures()   { return m_mapDissolveTextures; }
		const std::map<std::wstring, ComPtr<ID3D11ShaderResourceView>>& GetModelTextures()      { return m_mapModelTextures; }

	/********************
		Data Members
	********************/
	private:
		// 셰이더 리소스 뷰들
		std::map<std::wstring, ComPtr<ID3D11ShaderResourceView>> m_mapUITextures;
		std::map<std::wstring, ComPtr<ID3D11ShaderResourceView>> m_mapDiffuseTextures;
		std::map<std::wstring, ComPtr<ID3D11ShaderResourceView>> m_mapNoiseTextures;
		std::map<std::wstring, ComPtr<ID3D11ShaderResourceView>> m_mapMaskTextures;
		std::map<std::wstring, ComPtr<ID3D11ShaderResourceView>> m_mapDistortionTextures;
		std::map<std::wstring, ComPtr<ID3D11ShaderResourceView>> m_mapDissolveTextures;
		// 24.03.24 05:23 이준영
		// 모델에서 사용하는 메테리얼 ( 여러 장의 용도 별 텍스처 묶음 )
		std::map<std::wstring, ComPtr<ID3D11ShaderResourceView>> m_mapModelTextures;

		// 텍스처 파일 경로
		std::wstring m_wstrUITexturePath			= L"../../Resource/Textures/UI/";
		std::wstring m_wstrDiffuseTexturePath		= L"../../Resource/Textures/Diffuse/";
		std::wstring m_wstrNoiseTexturePath			= L"../../Resource/Textures/Noise/";
		std::wstring m_wstrMaskTexturePath			= L"../../Resource/Textures/Mask/";
		std::wstring m_wstrDistortionTexturePath	= L"../../Resource/Textures/Distortion/";
		std::wstring m_wstrDissolveTexturePath		= L"../../Resource/Textures/Dissolve/";
		
	private:
		ComPtr<ID3D11Device> m_cpDevice = nullptr;

		ComPtr<ID3D11DeviceContext> m_cpDeviceContext = nullptr;

		ComPtr<ID3D11DeviceContext> m_cpDeferredDeviceContext = nullptr;

		ComPtr<ID3D11Multithread> m_cpMultithread = nullptr;

	private:
		std::weak_ptr<CThreadManager> m_wpThreadManager;

		std::mutex m_mtMutex;
	};
}
