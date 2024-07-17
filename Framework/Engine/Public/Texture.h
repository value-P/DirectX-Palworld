#pragma once

namespace Engine
{
	class ENGINE_DLL CTexture final
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		CTexture() = default;

		virtual ~CTexture() = default;

	/********************
		Get
	********************/
	public:
		ComPtr<ID3D11ShaderResourceView> GetSRV() { return m_cpSRV; }

	/********************
		Events
	********************/
	public:
		static shared_ptr<CTexture> Create(const wstring& _wstrTextureFilePath);

		virtual void Initialize(const wstring& _wstrTextureFilePath);

	/********************
		Data Members
	********************/
	private:
		ComPtr<ID3D11ShaderResourceView> m_cpSRV = nullptr;
	};
}


