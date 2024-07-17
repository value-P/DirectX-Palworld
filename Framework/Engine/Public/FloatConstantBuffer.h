#pragma once
#include "ConstantBuffer.h"

namespace Engine
{
	class ENGINE_DLL CFloatConstantBuffer final : public CConstantBuffer
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CFloatConstantBuffer() = default;

		virtual ~CFloatConstantBuffer() = default;

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
		std::shared_ptr<FOneFloatConstantBuffer> m_spOneFloatConstantBuffer = nullptr;

		std::shared_ptr<FTwoFloatConstantBuffer> m_spTwoFloatConstantBuffer = nullptr;

		std::shared_ptr<FThreeFloatConstantBuffer> m_spThreeFloatConstantBuffer = nullptr;

		std::shared_ptr<FFourFloatConstantBuffer> m_spFourFloatConstantBuffer = nullptr;
	};
}
