#pragma once
#include "EffectBase.h"

namespace Engine
{
	class CParticleComponent;
}

namespace Client
{
	struct FParticleObjectDesc : public FEffectBaseDesc
	{

	};

	class CParticleObject final : public CEffectBase
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CParticleObject() = default;
		virtual ~CParticleObject() = default;
		
	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CParticleObject> Create(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc, FParticleInfo _tParticleInfo);
		static std::shared_ptr<CParticleObject> CreateFromJson(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc, const std::wstring& _wstrJsonFileName);

	public:
		virtual HRESULT Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc);
		HRESULT InitializeFromJson(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc, const std::wstring& _wstrJsonFileName);

		virtual HRESULT BeginPlay() override;
		virtual int32 PreTick(float _fDeltaSeconds) override;
		virtual int32 Tick(float _fDeltaSeconds) override;
		virtual int32 PostTick(float _fDeltaSeconds) override;
		virtual HRESULT EndPlay() override;
		virtual void Release() override;

	/********************
		Methods
	********************/
	public:
		/* 오브젝트 풀에서 꺼낼 때 불릴 함수 */
		void RecycleFromPool();

		/* 비활성화 시 실행 */
		void NoneActivate();

		/* 종료 시 실행 */
		void EndLifeTime();

		/* 루프 시 실행 */
		void Loop();


	/********************
		Getter / Setter
	********************/
	public:
		std::shared_ptr<CParticleComponent> GetParticleCom() { CHECKF(m_spParticle, L"Particle Component is nullptr : CParticleObject"); return m_spParticle; }

		uint32 GetCurKeyFrameIndex();
		std::vector<FParticleKeyFrame>& GetKeyFrames();

		void SetObjectParticleInfo(const FParticleInfo& _tParticleInfo) { m_tParticleInfo = _tParticleInfo; }

	/********************
		Data Members
	********************/
	private:
		/* 파티클 컴포넌트 */
		std::shared_ptr<CParticleComponent> m_spParticle;

		/* 파티클 Info */
		FParticleInfo m_tParticleInfo;

	private:
		ERenderType m_eRenderType = ERenderType::ENUM_END;
	};
}


