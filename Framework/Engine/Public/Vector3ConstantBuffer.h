#pragma once
#include "ConstantBuffer.h"

namespace Engine
{
	class CVector3ConstantBuffer : public CConstantBuffer
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CVector3ConstantBuffer() = default;
		
		virtual ~CVector3ConstantBuffer() = default;
		
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
		std::shared_ptr<FOneVector3ConstantBuffer> m_spOneVector3ConstantBuffer = nullptr;

		std::shared_ptr<FTwoVector3ConstantBuffer> m_spTwoVector3ConstantBuffer = nullptr;

		std::shared_ptr<FThreeVector3ConstantBuffer> m_spThreeVector3ConstantBuffer = nullptr;

		std::shared_ptr<FFourVector3ConstantBuffer> m_spFourVector3ConstantBuffer = nullptr;
	};
}
