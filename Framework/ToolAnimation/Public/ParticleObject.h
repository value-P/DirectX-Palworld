#pragma once
#include "EffectBase.h"

namespace Engine
{
	class CParticleComponent;
}

namespace ToolAnimation
{
	struct FParticleObjectDesc : FEffectBaseDesc
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
		/* ������Ʈ Ǯ���� ���� �� �Ҹ� �Լ� */
		void RecycleFromPool();

		/* ��Ȱ��ȭ �� ���� */
		void NoneActivate();

		/* ���� �� ���� */
		void EndLifeTime();

		/* ���� �� ���� */
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
		/* ��ƼŬ ������Ʈ */
		std::shared_ptr<CParticleComponent> m_spParticle;

		/* ��ƼŬ Info */
		FParticleInfo m_tParticleInfo;

	private:
		ERenderType m_eRenderType = ERenderType::ENUM_END;
	};
}


