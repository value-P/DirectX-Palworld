#pragma once
#include "ConstantBuffer.h"

namespace Engine
{
	class ENGINE_DLL CMatrixConstantBuffer final : public CConstantBuffer
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CMatrixConstantBuffer() = default;

		virtual ~CMatrixConstantBuffer() = default;
		
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
		std::shared_ptr<FOneMatrixConstantBuffer> m_spOneMatrixConstantBuffer = nullptr;

		std::shared_ptr<FTwoMatrixConstantBuffer> m_spTwoMatrixConstantBuffer = nullptr;

		std::shared_ptr<FThreeMatrixConstantBuffer> m_spThreeMatrixConstantBuffer = nullptr;

		std::shared_ptr<FFourMatrixConstantBuffer> m_spFourMatrixConstantBuffer = nullptr;
	};
}
