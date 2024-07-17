#pragma once
#include "Projectile.h"

namespace Engine
{
	class CPanel;
	class CProgressBarComponent;
	class CImageComponent;
	class CTextComponent;
	class CAnimationImageComponent;
	class CSoundManager;
}

namespace Client
{
	class CMonster;
	class CPalBoxSystem;
	class CUIManager;

    class CPalSphere : public CProjectile
    {
    /********************
		Enum
	********************/
	private:
		enum class EPalSphereState { OnThrow, OnMoveToPosition, OnLooping, OnSuccessCatch};

    /********************
		Ctor/Dtor
	********************/
	public:
		explicit CPalSphere() = default;

		virtual ~CPalSphere() = default;
		
	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CPalSphere> Create(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc);

		virtual HRESULT Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc) override;

		virtual HRESULT BeginPlay() override;

		virtual int32 PreTick(float _fDeltaSeconds) override;

		virtual int32 Tick(float _fDeltaSeconds) override;
		
		virtual int32 PostTick(float _fDeltaSeconds) override;

		virtual HRESULT EndPlay() override;

		virtual void Release() override;
		
	/********************
		Overrides
	********************/
	public:
		virtual void OnTriggerEnter(CComponent* _pSrcComponent, CComponent* _pDstComponent) override;
		
	private:
		virtual void InitTrailComponent() override;

	/********************
		Methods 
	********************/
	protected:
		virtual void ResetDataMember() override;

		// 포획확률을 랜덤으로 설정해줍니다.
		void ComputeCaptureProbability(int32 _iCapturePercentage);

		virtual void AnimNotifyCall(const list<FAnimNotifyData>& _lstAnimNotify) override;

	public:
		void PlaySoundWithID(const FAnimNotifyData& _tNotifyData);

		void PlayEffect(const FAnimNotifyData& _tNotifyData);

	/********************
		Data Members
	********************/
	private:
		bool m_bHit = false;

		bool m_bAnimationPlaying = false;

		Vector3 m_vDstPos = Vector3(0.0f);

		Vector3 m_vDstRelativePos = Vector3(1.0f, 2.f, 1.f);

	private:
		// 팰스피어의 상태
		EPalSphereState m_eState = EPalSphereState::OnThrow;

		// 팰 포획 관련 변수
		int32 m_iNumLoopCount = 0;// 몇 번 흔들릴지

		bool m_bSuccessCatch = false;// 포획 성공 여부

		float m_fTimeChecker = 0.0f;
		
		float m_fShakeWaitTime = 1.5f;
		
		float m_fSuccessTimeChecker = 0.f;

		bool m_bLoopEnded = false;

		bool m_bTriggerEnterCalled = false;

	private:
		// 팰 스피어에 맞은 야생 팰
		std::weak_ptr<CMonster> m_wpHitMonster;

		std::weak_ptr<CPalBoxSystem> m_wpPalBoxSystem;

	private:
		std::weak_ptr<CSoundManager> m_wpSoundManager;

		// 포획 UI 관련 변수
		std::weak_ptr<CUIManager> m_wpUIManager;

		// 포획 패널
		std::shared_ptr<CPanel> m_spCapturePanel = nullptr;
		
		// 포획 성공 패널
		std::shared_ptr<CPanel> m_spSuccessPanel = nullptr;
		
		std::shared_ptr<CAnimationImageComponent> m_spAnimation = nullptr;

		std::shared_ptr<CProgressBarComponent> m_spProgress = nullptr;

		std::shared_ptr<CImageComponent> m_spArrowImage = nullptr;

		std::shared_ptr <CTextComponent> m_spCaptureText = nullptr;

		std::vector<int32> m_vecCaptureProbability;

		int32 m_iCurLoopNum = 0;
    };
}