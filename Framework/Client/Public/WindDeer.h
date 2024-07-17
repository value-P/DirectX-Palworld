#pragma once
#include "RidingObject.h"

namespace Engine
{
	class CTrailComponent;
}

namespace Client
{
	class CEffectGroup;

	struct FWindDeerDesc : public FRidingObjectDesc
	{

	};

    class CWindDeer final : public CRidingObject
    {
    /********************
		Ctor/Dtor
	********************/
	public:
		explicit CWindDeer() = default;

		virtual ~CWindDeer() = default;

	/********************
		Events
	********************/
	public:
		static shared_ptr<CWindDeer> Create(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc);

		virtual HRESULT Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc) override;

		virtual HRESULT BeginPlay() override;

		virtual int32 PreTick(float _fDeltaSeconds) override;

		virtual int32 Tick(float _fDeltaSeconds) override;

		virtual int32 PostTick(float _fDeltaSeconds) override;

		virtual HRESULT EndPlay() override;

		virtual void Release() override;

	protected:
		virtual void InitRidingBone() override;
		virtual void InitMeshComponent() override;
		virtual void InitColliderComponent() override;
		virtual void InitFSMComponent() override;

		void InitRidingParticles();
		void InitRidingTrails();
		void InitCombinedBones();

	/********************
		Getter/Setter
	********************/
	public:
		virtual void Jump() override;
		void TickTrail(float _fTrailSize);

	/********************
		Methods
	********************/
	public:
		virtual void OnRidingEffect() override;
		virtual void OffRidingEffect() override;

	/********************
		Data Members
	********************/
	protected:
		// 라이딩 파티클(다리 뼈마다 1개씩)
		std::array<std::shared_ptr<CEffectGroup>, 4> m_arrRidingParticles;

		// 트레일
		std::array<std::shared_ptr<CTrailComponent>, 4> m_arrRidingTrails;

		// 트레일 on
		bool m_bActivateTrail = true;

		// 이펙트 부착용 본
		std::array<std::weak_ptr<CBone>, 4> m_arrCombinedBones;
    };
}