#pragma once
#include "Buffer.h"

namespace Engine
{
    class ENGINE_DLL CCubeBuffer final : public CBuffer
    {
    /********************
		Ctor/Dtor
	********************/
	public:
		explicit CCubeBuffer() = default;
		virtual ~CCubeBuffer() = default;

	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CCubeBuffer> Create();
		virtual HRESULT Initialize() override;
    };
}


