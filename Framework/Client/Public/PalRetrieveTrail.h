#pragma once
#include "EffectGroup.h"
#include "TrailComponent.h"

namespace Engine
{
	class CTrailComponent;
	class CBone;
}

namespace Client
{
	class CEffectManager;
	class CPlayer;

	struct FPalRetrieveTrailDesc : public CGameObject::FGameObjectDesc
	{
		
	};

	class CPalRetrieveTrail final : public CGameObject
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CPalRetrieveTrail() = default;
		virtual ~CPalRetrieveTrail() = default;
		
	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CPalRetrieveTrail> Create(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc);

	public:
		virtual HRESULT Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc);

		virtual HRESULT BeginPlay() override;

		virtual int32 PreTick(float _fDeltaSeconds) override;

		virtual int32 Tick(float _fDeltaSeconds) override;

		virtual int32 PostTick(float _fDeltaSeconds) override;

		virtual HRESULT EndPlay() override;

		virtual void Release() override;

	public:
		HRESULT InitializeTrailComponent();
		
	/********************
		Methods
	********************/
	public:
		void TickTrail(float _fTrailSize, float _fDeltaSeconds);
		void OnTrail(const Vector3& _vStartPos, const Vector3 _vEndPos, float _fTrailSize);
		void OffTrail();

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

		// 트레일
		std::shared_ptr<CTrailComponent> m_spTrailCom = nullptr;

	private:
		// 트레일 활성화
		bool m_bActiveTrail = true;

		// 트레일 크기
		float m_fTrailSize = 0.1f;

		// 트레일 활성화 시간
		float m_fTrailActiveAcc = 0.f;

		// 시작 지점
		Vector3 m_vStartPos = Vector3(0.f);

		// 끝 지점
		Vector3 m_vEndPos = Vector3(0.f);

		// 플레이어
		weak_ptr<CPlayer> m_wpPlayer;

		// 플레이어 손 뼈
		weak_ptr<CBone> m_wpPlayerHandBone;
	};
}