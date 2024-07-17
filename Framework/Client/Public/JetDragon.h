#pragma once
#include "RidingObject.h"

namespace Engine
{
	class CBone;
	class CTrailComponent;
}

namespace Client
{
	class CEffectGroup;

	struct FJetDragonDesc : public FRidingObjectDesc
	{

	};

    class CJetDragon final : public CRidingObject
    {
    /********************
		Ctor/Dtor
	********************/
	public:
		explicit CJetDragon() = default;

		virtual ~CJetDragon() = default;

	/********************
		Events
	********************/
	public:
		static shared_ptr<CJetDragon> Create(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc);

		virtual HRESULT Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc) override;

		virtual HRESULT BeginPlay() override;

		virtual int32 PreTick(float _fDeltaSeconds) override;

		virtual int32 Tick(float _fDeltaSeconds) override;

		virtual int32 PostTick(float _fDeltaSeconds) override;

		virtual HRESULT EndPlay() override;

		virtual void Release() override;

	protected:
		virtual void KeyInput() override;
		void TickTrail(float _fTrailSize);

	protected:
		virtual void InitRidingBone() override;
		virtual void InitMeshComponent() override;
		virtual void InitColliderComponent() override;
		virtual void InitFSMComponent() override;
		void InitRidingParticles();
		void InitRidingTrails();
		void InitCombinedBones();

	public:
		virtual void OnRidingEffect() override;
		virtual void OffRidingEffect() override;


	/********************
		Getter/Setter
	********************/
	public:

	/********************
		Methods
	********************/
	public:

	/********************
		Data Members
	********************/
	private:
		// 비행 파티클(날개 뼈마다 1개씩)
		std::array<std::shared_ptr<CEffectGroup>, 4> m_arrFlyingParticles;

		// 트레일
		std::array<std::shared_ptr<CTrailComponent>, 2> m_arrFlyingTrails;

		// 트레일 on
		bool m_bActivateTrail = true;

		// 이펙트 부착용 본
		std::array<std::weak_ptr<CBone>, 4> m_arrCombinedBones;
    };
}