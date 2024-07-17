#pragma once
#include "EffectGroup.h"
#include "TrailComponent.h"

namespace Engine
{
	class CModel;
}

namespace Client
{
	class CEffectManager;
	class CCreature;

	struct FPalDeadParticleDesc : public FEffectGroupDesc
	{
		
	};

	class CPalDeadParticle final : public CEffectGroup
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CPalDeadParticle() = default;
		virtual ~CPalDeadParticle() = default;
		
	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CPalDeadParticle> Create(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc);
		static std::shared_ptr<CPalDeadParticle> CreateFromJson(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc, const std::wstring& _wstrJsonFileName);

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
		void ResetAtBonePosition(std::shared_ptr<CGameObject> _spGameObject, std::shared_ptr<CModel> _spModel);


	/********************
		Getter / Setter
	********************/
	public:
		

	/********************
		Data Members
	********************/
	private:
		// 이펙트 매니저
		std::weak_ptr<CEffectManager> m_wpEffectManager;
	};
}