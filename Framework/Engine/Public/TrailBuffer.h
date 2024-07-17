#pragma once
#include "Buffer.h"

namespace Engine
{
	struct FTrailBufferDesc
	{
		uint32 m_iNumRect = 0;
	};

	class ENGINE_DLL CTrailBuffer final : public CBuffer
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CTrailBuffer() = default;
		virtual ~CTrailBuffer() = default;

	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CTrailBuffer> Create(const std::shared_ptr<FTrailBufferDesc>& _spTrailBufferDesc);

		HRESULT Initialize(const std::shared_ptr<FTrailBufferDesc>& _spTrailBufferDesc);

		// virtual void Release() override;

	/********************
		Getter / Setter
	********************/
	public:
		ComPtr<ID3D11Buffer>& GetVertexBuffer() { CHECKF(m_cpVertexBuffer, L"VertexBuffer is nullptr : CTrailBuffer"); return m_cpVertexBuffer; }
		uint32 GetNumVertices() { return m_iNumVertices; }

	/********************
		Data Members
	********************/
	protected:
		/* 트레일로 그릴 사각형의 개수 */
		uint32 m_iNumRect = 0;
	};
}