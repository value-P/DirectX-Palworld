#pragma once
#include "Buffer.h"

namespace Engine
{
    class ENGINE_DLL CPointBuffer final : public CBuffer
    {
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CPointBuffer() = default;

		virtual ~CPointBuffer() = default;

	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CPointBuffer> Create();

		virtual HRESULT Initialize() override;

		virtual HRESULT Render() override;

		virtual void Release() override;

		virtual HRESULT BindBuffers() override;
    };
}


