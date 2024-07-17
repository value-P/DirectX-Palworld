#pragma once

namespace Engine
{
	class CDeviceManager;

	class ENGINE_DLL CConstantBuffer
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CConstantBuffer() = default;

		virtual ~CConstantBuffer() = default;
		
	/********************
		Events
	********************/
	public:
		template<typename T, typename = std::enable_if_t<std::is_base_of_v<CConstantBuffer, T>>>
		static std::shared_ptr<T> Create(int32 _iBindPoint, EConstantBufferType _eConstantBufferType)
		{
			const shared_ptr<T> spBuffer = make_shared<T>();
			ENSUREF(spBuffer->Initialize(_iBindPoint, _eConstantBufferType), L"Failed to initialize constant buffer.");
			return spBuffer;
		}

		virtual HRESULT Initialize(int32 _iBindPoint, EConstantBufferType _eConstantBufferType);

		virtual void Release();

	/******************
		Methods
	******************/
	public:
		void VSSetConstantBuffers();

		void PSSetConstantBuffers();

		void GSSetConstantBuffers();

	/********************
		Data Members
	********************/
	protected:
		std::weak_ptr<CDeviceManager> m_wpDeviceManager;

	protected:
		ComPtr<ID3D11Buffer> m_cpConstantBuffer = nullptr;

		int32 m_iRegister = -1;

		bool m_bDirty = false;

	protected:
		EConstantBufferType m_eConstantBufferType = EConstantBufferType::ENUM_END;

		EBufferType m_eBufferType = EBufferType::CONSTANT;

		std::wstring m_wstrConstantBufferName = L"";
	};
}
