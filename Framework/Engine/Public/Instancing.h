#pragma once
#include "Buffer.h"
#include "ParticleComponent.h"

namespace Engine
{
	class ENGINE_DLL CInstancing final : public CBuffer
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CInstancing() = default;
		virtual ~CInstancing() = default;

	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CInstancing> Create(FParticleInfo _tParticleInfo);

		virtual HRESULT Initialize() override;

		virtual HRESULT Render() override;

	protected:
		virtual HRESULT BindBuffers();

	/********************
		Getter / Setter
	********************/
	public:
		ComPtr<ID3D11Buffer>& GetInstanceBuffer() { CHECKF(m_cpInstanceBuffer, L"InstanceBuffer is nullptr : CInstancing"); return m_cpInstanceBuffer; }

	public:
		void SetParticleInfo(const FParticleInfo& _tParticleInfo) { m_tParticleInfo = _tParticleInfo; }
		const FParticleInfo& GetParticleInfo() { return m_tParticleInfo; }

	/********************
		Data Members
	********************/
		/* 인스턴스 버퍼 */
		ComPtr<ID3D11Buffer> m_cpInstanceBuffer = nullptr;

	protected:
		/* 파티클 Desc */
		FParticleInfo m_tParticleInfo;

		uint32 m_iInstanceStride = 0;

		uint32 m_iIndexCountPerInstance = 0;
	};
}
