#pragma once
#include "Buffer.h"

namespace Engine
{
    class ENGINE_DLL CRectBuffer final : public CBuffer
    {
    /********************
		Ctor/Dtor
	********************/
	public:
		explicit CRectBuffer() = default;
		virtual ~CRectBuffer() = default;

	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CRectBuffer> Create();
		virtual HRESULT Initialize() override;

    };
}


