#pragma once
#include "ConstantBuffer.h"

namespace Engine
{
	class CVector4ConstantBuffer : public CConstantBuffer
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CVector4ConstantBuffer() = default;
		
		virtual ~CVector4ConstantBuffer() = default;
		
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
		std::shared_ptr<FOneVector4ConstantBuffer> m_spOneVector4ConstantBuffer = nullptr;

		std::shared_ptr<FTwoVector4ConstantBuffer> m_spTwoVector4ConstantBuffer = nullptr;

		std::shared_ptr<FThreeVector4ConstantBuffer> m_spThreeVector4ConstantBuffer = nullptr;

		std::shared_ptr<FFourVector4ConstantBuffer> m_spFourVector4ConstantBuffer = nullptr;
	};
}
