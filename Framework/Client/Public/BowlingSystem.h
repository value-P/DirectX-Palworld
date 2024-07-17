#pragma once

namespace Engine
{
	class CSoundManager;
	class CInputManager;
	class CPanel;
	class CAnimationImageComponent;
	class CImageComponent;
	class CTextComponent;
	class CCamera;
	class CGameObject;
	class CModel;
}

namespace Client
{
	class CBowl;
	class CBowlingPin;
	class CBowlingTrigger;
	class CUIManager;
	class CBowlingCamera;
	class CBowlingFloorObject;
	class CBowlingPinAppearParticle;

	class CNpc_Quest03;

	/********************
		Descriptor
	********************/
	struct FBowlingSystemDesc final
	{
	};

	struct FScoreInfo final
	{
		std::shared_ptr<CTextComponent> m_spTurn0_0Text;
		std::shared_ptr<CTextComponent> m_spTurn0_1Text;
		
		std::shared_ptr<CTextComponent> m_spTurn1_0Text;
		std::shared_ptr<CTextComponent> m_spTurn1_1Text;
		
		std::shared_ptr<CTextComponent> m_spTurn2_0Text;
		std::shared_ptr<CTextComponent> m_spTurn2_1Text;

	};

	class CBowlingSystem final
	{
		enum ETurnType
		{
			PLAYER,
			AI,
			ENUM_END
		};

	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CBowlingSystem() = default;

		virtual ~CBowlingSystem() = default;

	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CBowlingSystem> Create(const std::shared_ptr<FBowlingSystemDesc>& _spBowlingSystemDesc);

		HRESULT Initialize(const std::shared_ptr<FBowlingSystemDesc>& _spBowlingSystemDesc);

		HRESULT BeginPlay();

		int32 PreTick(float _fDeltaSeconds);

		int32 Tick(float _fDeltaSeconds);
		
		int32 PostTick(float _fDeltaSeconds);

		HRESULT EndPlay();

		void Release();

		void InitCamera();

		void InitApearParticle();

	/********************
		Getter/Setter
	********************/
	public:
		void SetNext(bool _bNext) { m_bNext = _bNext; }

		bool IsFinish() { return m_bFinish; }

		bool IsVictory() { return m_iPlayerSuccessNum >= m_iAISuccessNum; }

	/********************
		Methods
	********************/
	public:
		void PerformTurn(float _fDeltaSeconds);

		void SelectPos(float _fDeltaSeconds);

		void SelectAngle(float _fDeltaSeconds);

		void SelectPower(float _fDeltaSeconds);

		void ResetBowl();

		void ResetBowlingPin();

		void DelayResetBowl(float _fDeltaSeconds);

		void DelayResetBowlingPin(float _fDeltaSeconds);

		void NextTurn();

		void CheckRound();

		void CheckBowlingPin();

		void AddPanels();

		void SaveTurnScore(ETurnType _eTurnType);

		void SetGameStart();

		shared_ptr<CBowlingPinAppearParticle> ActiveBowlingPinParticle(std::shared_ptr<CGameObject> _spGameObject, std::shared_ptr<CModel> _spModel);

	public:
		// 볼링 시스템에 영향을 받는 npc의 포인터를 저장합니다.
		void SetNPC(std::weak_ptr<CNpc_Quest03> _wpNPC) { m_wpNPC = _wpNPC; }

	/********************
		Data Members
	********************/
	private:
		// 볼링공
		std::shared_ptr<CBowl> m_spBowl;
		// 볼링핀
		std::vector<std::shared_ptr<CBowlingPin>> m_vecBowlingPin;
		std::vector<Vector3> m_vecBowlingPinPos;

		std::vector<std::shared_ptr<CBowlingTrigger>> m_vecBowlingTriggers;

	private:
		// 라운드 횟수
		int32 m_iCurRoundNum = 1;
		int32 m_iMaxRoundNum = 4;

		int32 m_iRoundCount = 0;

		// 현재 턴이 누구인지 저장합니다.
		ETurnType m_eTurnType = ETurnType::PLAYER;

		// 한 라운드 당 공을 던지는 횟수
		int32 m_iTurnNum = 2;
		int32 m_iCurTurnNum = 0;

		// 총 라운드 당 턴을 진행한 횟수를 저장합니다.
		int32 m_iPlayerTurnCount = 0;
		int32 m_iNPCTurnCount = 0;

		// 점수를 저장하기 위한 변수입니다.
		int32 m_iPlayerSuccessNum = 0;
		int32 m_iAISuccessNum = 0;

		// 자신의 턴동안 친 볼링핀의 개수를 저장합니다.
		int32 m_iBowlingPinNum = 0;
		// 턴 하나당 친 개수를 저장합니다.
		int32 m_iBowlingPinTurn1Num = 0;
		int32 m_iBowlingPinTurn2Num = 0;

		// 위치, 방향, 세기를 정했는지 체크합니다.
		int32 m_iSelectNum = 0;

		// 시작 위치를 저장합니다.
		Vector3 m_vStartPos;
		// 끝 위치를 저장합니다.
		Vector3 m_vEndPos;
		// 볼링공 시작 위치를 저장합니다.
		Vector3 m_vPos;
		// 방향을 저장합니다.
		float m_fAngle = 0.0f;
		// 세기를 저장합니다.
		float m_fPower = 4.0f;
		
		// AI 위치, 방향, 세기
		float m_fPosTime = 0.0f;
		float m_fAngleTime = 0.0f;
		float m_fPowerTime = 0.0f;

		float m_fAISelectTime = 0.0f;

		// 볼을 리셋하는데 걸리는 시간을 저장합니다.
		float m_fDelayBowlTime = 0.0f;
		// 볼링핀을 리셋하는데 걸리는 시간을 저장합니다.
		float m_fDelayBowlingPinTime = 0.0f;
		// 리셋하는 시간을 저장합니다. 
		float m_fLimitTime = 4.0f;

		// 턴 유예 시간을 저장합니다.
		float m_fDelayTurnTime = 0.0f;

		bool m_bFirst = false;

		bool m_bNext = false;

		bool m_bDelayBowl = false;
		bool m_bDelayBowlingPin = false;

		bool m_bLeft = false;
		bool m_bAngleLeft = false;
		bool m_bUp = false;
		// 게임중인지 체크합니다.
		bool m_bPlaying = false;
		// 게임이 끝났는지 체크합니다.
		bool m_bFinish = false;
		// npc와 연동할 시작 이벤트입니다.
		bool m_bStartEvent = false;

		//
		bool m_bRollSound = false;

	private:
		std::weak_ptr<CInputManager> m_wpInputManager;
		std::weak_ptr<CUIManager> m_wpUIManager;
		std::weak_ptr<CSoundManager> m_wpSoundManager;
		// UI
	private:
		// hud를 저장합니다.
		std::shared_ptr<CPanel> m_spBowlingHUDPanel;
		// 플레이어 차례 애니메이션을 저장합니다.
		std::shared_ptr<CAnimationImageComponent> m_spPlayerAnimation;
		// npc 차례 애니메이션을 저장합니다.
		std::shared_ptr<CAnimationImageComponent> m_spNPCAnimation;
		std::shared_ptr<CImageComponent> m_spRoundImage;
		// 점수를 표시하기위한 텍스트 컴포넌트들을 저장합니다.
		FScoreInfo m_tPlayerScoreInfo;
		FScoreInfo m_tNPCScoreInfo;
		// 볼링공의 세기 패널을 저장합니다.
		std::shared_ptr<CPanel> m_spPowerPanel;
		std::shared_ptr<CImageComponent> m_spPowerBarImage;
		// 볼링공의 각도 패널을 저장합니다.
		std::shared_ptr<CPanel> m_spAnglePanel;
		std::shared_ptr<CImageComponent> m_spAngleImage;
		// 볼링공의 위치 패널을 저장합니다.
		std::shared_ptr<CPanel> m_spPositionPanel;
		std::shared_ptr<CImageComponent> m_spPositionImage;

		// 볼링 게임 시작, 승리, 패배를 보여줄 패널을 저장합니다.
		std::shared_ptr<CPanel> m_spGamePanel;
		std::shared_ptr<CAnimationImageComponent> m_spGameAnimation;

		std::wstring m_wstrGameStart = L"T_MiniGame_GameStartImage";
		std::wstring m_wstrGameFail = L"T_MiniGame_GameFailImage";
		std::wstring m_wstrGameSuccess = L"T_MiniGame_GameSuccessImage";

	private:
		//std::shared_ptr<CCamera> m_spBowlingCamera = nullptr;
		std::shared_ptr<CBowlingCamera> m_spBowlingCamera = nullptr;

		Vector3 m_vBowlingCameraPos = { 0.f, 0.f, 0.f };
		Vector3 m_vBowlingLookAt = { 0.f, 0.f, 0.f };

	private:
		Vector3 m_vAIPos;
		float m_fAIAngle;
		float m_fAIPower;

	private:
		std::weak_ptr<CNpc_Quest03> m_wpNPC;

	private:
		std::shared_ptr<CBowlingFloorObject> m_spFloorObject;

	private:
		// 볼링핀 이펙트
		std::array<std::shared_ptr<CBowlingPinAppearParticle>, 11> m_arrBowlingPinAppearEffects;
	};
}