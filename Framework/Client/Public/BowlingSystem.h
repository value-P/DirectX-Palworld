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
		// ���� �ý��ۿ� ������ �޴� npc�� �����͸� �����մϴ�.
		void SetNPC(std::weak_ptr<CNpc_Quest03> _wpNPC) { m_wpNPC = _wpNPC; }

	/********************
		Data Members
	********************/
	private:
		// ������
		std::shared_ptr<CBowl> m_spBowl;
		// ������
		std::vector<std::shared_ptr<CBowlingPin>> m_vecBowlingPin;
		std::vector<Vector3> m_vecBowlingPinPos;

		std::vector<std::shared_ptr<CBowlingTrigger>> m_vecBowlingTriggers;

	private:
		// ���� Ƚ��
		int32 m_iCurRoundNum = 1;
		int32 m_iMaxRoundNum = 4;

		int32 m_iRoundCount = 0;

		// ���� ���� �������� �����մϴ�.
		ETurnType m_eTurnType = ETurnType::PLAYER;

		// �� ���� �� ���� ������ Ƚ��
		int32 m_iTurnNum = 2;
		int32 m_iCurTurnNum = 0;

		// �� ���� �� ���� ������ Ƚ���� �����մϴ�.
		int32 m_iPlayerTurnCount = 0;
		int32 m_iNPCTurnCount = 0;

		// ������ �����ϱ� ���� �����Դϴ�.
		int32 m_iPlayerSuccessNum = 0;
		int32 m_iAISuccessNum = 0;

		// �ڽ��� �ϵ��� ģ �������� ������ �����մϴ�.
		int32 m_iBowlingPinNum = 0;
		// �� �ϳ��� ģ ������ �����մϴ�.
		int32 m_iBowlingPinTurn1Num = 0;
		int32 m_iBowlingPinTurn2Num = 0;

		// ��ġ, ����, ���⸦ ���ߴ��� üũ�մϴ�.
		int32 m_iSelectNum = 0;

		// ���� ��ġ�� �����մϴ�.
		Vector3 m_vStartPos;
		// �� ��ġ�� �����մϴ�.
		Vector3 m_vEndPos;
		// ������ ���� ��ġ�� �����մϴ�.
		Vector3 m_vPos;
		// ������ �����մϴ�.
		float m_fAngle = 0.0f;
		// ���⸦ �����մϴ�.
		float m_fPower = 4.0f;
		
		// AI ��ġ, ����, ����
		float m_fPosTime = 0.0f;
		float m_fAngleTime = 0.0f;
		float m_fPowerTime = 0.0f;

		float m_fAISelectTime = 0.0f;

		// ���� �����ϴµ� �ɸ��� �ð��� �����մϴ�.
		float m_fDelayBowlTime = 0.0f;
		// �������� �����ϴµ� �ɸ��� �ð��� �����մϴ�.
		float m_fDelayBowlingPinTime = 0.0f;
		// �����ϴ� �ð��� �����մϴ�. 
		float m_fLimitTime = 4.0f;

		// �� ���� �ð��� �����մϴ�.
		float m_fDelayTurnTime = 0.0f;

		bool m_bFirst = false;

		bool m_bNext = false;

		bool m_bDelayBowl = false;
		bool m_bDelayBowlingPin = false;

		bool m_bLeft = false;
		bool m_bAngleLeft = false;
		bool m_bUp = false;
		// ���������� üũ�մϴ�.
		bool m_bPlaying = false;
		// ������ �������� üũ�մϴ�.
		bool m_bFinish = false;
		// npc�� ������ ���� �̺�Ʈ�Դϴ�.
		bool m_bStartEvent = false;

		//
		bool m_bRollSound = false;

	private:
		std::weak_ptr<CInputManager> m_wpInputManager;
		std::weak_ptr<CUIManager> m_wpUIManager;
		std::weak_ptr<CSoundManager> m_wpSoundManager;
		// UI
	private:
		// hud�� �����մϴ�.
		std::shared_ptr<CPanel> m_spBowlingHUDPanel;
		// �÷��̾� ���� �ִϸ��̼��� �����մϴ�.
		std::shared_ptr<CAnimationImageComponent> m_spPlayerAnimation;
		// npc ���� �ִϸ��̼��� �����մϴ�.
		std::shared_ptr<CAnimationImageComponent> m_spNPCAnimation;
		std::shared_ptr<CImageComponent> m_spRoundImage;
		// ������ ǥ���ϱ����� �ؽ�Ʈ ������Ʈ���� �����մϴ�.
		FScoreInfo m_tPlayerScoreInfo;
		FScoreInfo m_tNPCScoreInfo;
		// �������� ���� �г��� �����մϴ�.
		std::shared_ptr<CPanel> m_spPowerPanel;
		std::shared_ptr<CImageComponent> m_spPowerBarImage;
		// �������� ���� �г��� �����մϴ�.
		std::shared_ptr<CPanel> m_spAnglePanel;
		std::shared_ptr<CImageComponent> m_spAngleImage;
		// �������� ��ġ �г��� �����մϴ�.
		std::shared_ptr<CPanel> m_spPositionPanel;
		std::shared_ptr<CImageComponent> m_spPositionImage;

		// ���� ���� ����, �¸�, �й踦 ������ �г��� �����մϴ�.
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
		// ������ ����Ʈ
		std::array<std::shared_ptr<CBowlingPinAppearParticle>, 11> m_arrBowlingPinAppearEffects;
	};
}