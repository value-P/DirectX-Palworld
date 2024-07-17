#pragma once
#include "ConstantBuffer.h"

namespace Engine
{
	class ENGINE_DLL CIntConstantBuffer final : public CConstantBuffer
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CIntConstantBuffer() = default;
		
		virtual ~CIntConstantBuffer() = default;
		
	/********************
		Events
	********************/
	public:
		virtual HRESULT Initialize(int32 _iRegister, EConstantBufferType _eConstantBufferType) override;

		virtual void Release() override;

	/********************
		Methods
	********************/
	public:
		void TickConstantBuffer(const std::shared_ptr<FConstantBuffer>& _spConstantBuffer);

		void CopyConstantBuffer();
		
	protected:
		std::shared_ptr<FOneIntConstantBuffer> m_spOneIntConstantBuffer = nullptr;

		std::shared_ptr<FTwoIntConstantBuffer> m_spTwoIntConstantBuffer = nullptr;

		std::shared_ptr<FThreeIntConstantBuffer> m_spThreeIntConstantBuffer = nullptr;

		std::shared_ptr<FFourIntConstantBuffer> m_spFourIntConstantBuffer = nullptr;
	};
}
