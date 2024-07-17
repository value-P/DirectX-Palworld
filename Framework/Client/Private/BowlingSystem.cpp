#include "pch.h"
#include "BowlingSystem.h"

#include "Bowl.h"
#include "BowlingPin.h"
#include "BowlingTrigger.h"

#include "ClientModule.h"
#include "GameManager.h"
#include "UIManager.h"
#include "EffectManager.h"

#include "Level.h"
#include "World.h"

#include "MathUtility.h"
#include "EngineModule.h"
#include "InputManager.h"
#include "SoundManager.h"

#include "Panel.h"
#include "Camera.h"

#include "TextComponent.h"
#include "ImageComponent.h"
#include "AnimationImageComponent.h"
#include "MeshComponent.h"
#include "Model.h"

#include "BowlingCamera.h"
#include "Player.h"

#include "Npc_Quest03.h"
#include "BowlingFloorObject.h"
#include "BowlingPinAppearParticle.h"

std::shared_ptr<CBowlingSystem> Client::CBowlingSystem::Create(const std::shared_ptr<FBowlingSystemDesc>& _spBowlingSystemDesc)
{
	std::shared_ptr<CBowlingSystem> spBowlingSystem = make_shared<CBowlingSystem>();
	CHECKF(spBowlingSystem, L"CBowlingSystem : Create Failed");
	spBowlingSystem->Initialize(_spBowlingSystemDesc);
	return spBowlingSystem;
}

HRESULT Client::CBowlingSystem::Initialize(const std::shared_ptr<FBowlingSystemDesc>& _spBowlingSystemDesc)
{
	m_wpSoundManager = CEngineModule::GetInstance()->GetSoundManager();

	m_vStartPos = Vector3(-3396.3f, 10.3f, -2491.8f);
	m_vEndPos = Vector3(-3396.3f, 10.3f, -2495.2f);

	// ��ġ ������ �ʿ��մϴ�.
	// �������� �������� �����մϴ�.
	FBowlDesc tBowlDesc;

	tBowlDesc.m_bActive = true;
	tBowlDesc.m_wstrGameObjectName = L"Bowl";
	tBowlDesc.m_vInitPosition = Vector3(-3396.3f, 10.3f, -2493.5f);

	m_spBowl = CBowl::Create(make_shared<FBowlDesc>(tBowlDesc));

	// ������ �Ҽӽ�ŵ�ϴ�.
	CWorld::GetInstance()->GetPostFrontLevel()->AddGameObject(m_spBowl);

	// 10�� ���� �����Դϴ�.
	for (int32 i = 0; i < 10; i++)
	{
		FBowlingPinDesc tBowlingPinDesc;
		tBowlingPinDesc.m_bActive = true;
		tBowlingPinDesc.m_wstrGameObjectName = L"BowlingPin" + to_wstring(i);
		if (0 == i)
		{
			tBowlingPinDesc.m_vInitPosition = Vector3(-3379.8f, 9.8f, -2493.5f);
		}
		//
		else if (1 == i)
		{
			tBowlingPinDesc.m_vInitPosition = Vector3(-3378.8f, 9.8f, -2492.7f);
		}
		else if (2 == i)
		{
			tBowlingPinDesc.m_vInitPosition = Vector3(-3378.8f, 9.8f, -2494.3f);
		}
		//
		else if (3 == i)
		{
			tBowlingPinDesc.m_vInitPosition = Vector3(-3377.8f, 9.8f, -2491.9f);
		}
		else if (4 == i)
		{
			tBowlingPinDesc.m_vInitPosition = Vector3(-3377.8f, 9.8f, -2493.5f);
		}
		else if (5 == i)
		{
			tBowlingPinDesc.m_vInitPosition = Vector3(-3377.8f, 9.8f, -2495.1f);
		}
		//
		else if (6 == i)
		{
			tBowlingPinDesc.m_vInitPosition = Vector3(-3376.8f, 9.8f, -2491.1f);
		}
		else if (7 == i)
		{
			tBowlingPinDesc.m_vInitPosition = Vector3(-3376.8f, 9.8f, -2492.7f);
		}
		else if (8 == i)
		{
			tBowlingPinDesc.m_vInitPosition = Vector3(-3376.8f, 9.8f, -2494.3f);
		}
		else if (9 == i)
		{
			tBowlingPinDesc.m_vInitPosition = Vector3(-3376.8f, 9.8f, -2495.9f);
		}

		m_vecBowlingPinPos.push_back(tBowlingPinDesc.m_vInitPosition);
		std::shared_ptr<CBowlingPin> spBowlingPin = CBowlingPin::Create(make_shared<FBowlingPinDesc>(tBowlingPinDesc));
		// ������ �Ҽӽ�ŵ�ϴ�.
		CWorld::GetInstance()->GetPostFrontLevel()->AddGameObject(spBowlingPin);
		m_vecBowlingPin.push_back(spBowlingPin);
	}


	for (int32 i = 0; i < 3; i++)
	{
		FBowlingTriggerDesc tBowlingTriggerDesc;
		tBowlingTriggerDesc.m_wstrGameObjectName = L"BowlingTrigger" + to_wstring(i);
		if (0 == i)
		{
			tBowlingTriggerDesc.m_vInitPosition = Vector3(-3382.4f, 10.5f, -2488.1f);
			tBowlingTriggerDesc.m_vExtents = Vector3(12.f, 8.f, 1.f);
		}
		else if (1 == i)
		{
			tBowlingTriggerDesc.m_vInitPosition = Vector3(-3382.4f, 10.5f, -2498.1f);
			tBowlingTriggerDesc.m_vExtents = Vector3(12.f, 8.f, 1.f);
		}
		else if (2 == i)
		{
			tBowlingTriggerDesc.m_vInitPosition = Vector3(-3370.f, 10.5f, -2493.3f);
			tBowlingTriggerDesc.m_vExtents = Vector3(4.f, 8.f, 7.f);
		}

		std::shared_ptr<CBowlingTrigger> spBowlingTrigger = CBowlingTrigger::Create(make_shared<FBowlingTriggerDesc>(tBowlingTriggerDesc));
		CWorld::GetInstance()->GetPostFrontLevel()->AddGameObject(spBowlingTrigger);
		m_vecBowlingTriggers.push_back(spBowlingTrigger);
	}

	m_wpUIManager = CUIManager::GetInstance();
	AddPanels();
	//m_spBowlingHUDPanel->SetActive(true);
	//m_wpUIManager.lock()->FindParentsPanel(L"HUD")->SetActive(false);

	// ī�޶� �߰��մϴ�.
	InitCamera();

	// �ٴ� �ݶ��̴��� �߰��մϴ�.
	FBowlingFloorObject tBowlingFloorObject;

	tBowlingFloorObject.m_bActive = true;
	tBowlingFloorObject.m_wstrGameObjectName = L"BowlingFloorObject";
	tBowlingFloorObject.m_vInitPosition = Vector3(-3384.9f, 9.15f, -2488.f);
	m_spFloorObject = CBowlingFloorObject::Create(make_shared<FBowlingFloorObject>(tBowlingFloorObject));
	CWorld::GetInstance()->GetPostFrontLevel()->AddGameObject(m_spFloorObject);

	InitApearParticle();

	return S_OK;
}

HRESULT Client::CBowlingSystem::BeginPlay()
{
	m_wpInputManager = CEngineModule::GetInstance()->GetInputManager();

	return S_OK;
}

int32 Client::CBowlingSystem::PreTick(float _fDeltaSeconds)
{
	return 0;
}

int32 Client::CBowlingSystem::Tick(float _fDeltaSeconds)
{
	if (m_bStartEvent && false == m_bFinish)
	{
		m_spBowl->RequestTransform(std::nullopt, Vector3(0.f, 90.f, 0.f));
		for (auto& iter : m_vecBowlingPin)
		{
			iter->RequestTransform(std::nullopt, Vector3(0.f, 90.f, 0.f));
		}

		// ���� ���� �� hud���� ���ݴϴ�.
		m_wpUIManager.lock()->GetHUDParentPanel()->SetActive(false);
		m_wpUIManager.lock()->FindParentsPanel(L"RidingUI")->SetActive(false);

		if (false == m_spGamePanel->IsActive())
		{
			m_spGamePanel->SetActive(true);
			m_wpSoundManager.lock()->PlaySoundEx("TurnChangeSound");
		}

		// ī�޶� �ٲ��ݴϴ�.
		CGameManager::GetInstance()->GetPlayer()->SetActive(false);
		m_spBowlingCamera->SetOnAir();
		m_vBowlingCameraPos = m_spBowl->GetPosition().value() - Vector3(4.5f, -2.5f, 0.f);
		//m_vBowlingCameraPos = m_vecBowlingPinPos[0] - Vector3(3.5f, -2.5f, 0.f);
		m_spBowlingCamera->SetPosition(m_vBowlingCameraPos);
		//m_vBowlingLookAt = m_spBowl->GetPosition().value();
		m_vBowlingLookAt = m_vecBowlingPinPos[0];
		m_spBowlingCamera->LookAt(m_vBowlingLookAt);

		if (true == m_spGameAnimation->IsFinish())
		{
			// ���� bgm�� ������ݴϴ�.
			m_wpSoundManager.lock()->StopChannelEx(EChannelType::BACKGROUND_00);
			m_wpSoundManager.lock()->PlayBGM("BGM_Bowling");

			// ���� UI�� �մϴ�.
			m_spBowlingHUDPanel->SetActive(true);

			m_spGameAnimation->ResetFrame();
			m_spGamePanel->SetActive(false);
			m_bStartEvent = false;
			m_bPlaying = true;

			// �ʱ�ȭ������ϴ� ��
			{
				m_iCurRoundNum = 1;
				m_iRoundCount = 0;
				m_iPlayerTurnCount = 0;
				m_iNPCTurnCount = 0;
				m_iPlayerSuccessNum = 0;
				m_iAISuccessNum = 0;
				m_iSelectNum = 0;
				m_fAngle = 0;
				m_fPower = 4.f;
				m_fAISelectTime = 0.0f;
				m_fDelayBowlTime = 0.0f;
				m_fDelayBowlingPinTime = 0.0f;
			}

			{
				m_tPlayerScoreInfo.m_spTurn0_0Text->SetText(L"00");
				m_tPlayerScoreInfo.m_spTurn0_1Text->SetText(L"00");

				m_tPlayerScoreInfo.m_spTurn1_0Text->SetText(L"00");
				m_tPlayerScoreInfo.m_spTurn1_1Text->SetText(L"00");

				m_tPlayerScoreInfo.m_spTurn2_0Text->SetText(L"00");
				m_tPlayerScoreInfo.m_spTurn2_1Text->SetText(L"00");
			}

			{
				m_tNPCScoreInfo.m_spTurn0_0Text->SetText(L"00");
				m_tNPCScoreInfo.m_spTurn0_1Text->SetText(L"00");

				m_tNPCScoreInfo.m_spTurn1_0Text->SetText(L"00");
				m_tNPCScoreInfo.m_spTurn1_1Text->SetText(L"00");

				m_tNPCScoreInfo.m_spTurn2_0Text->SetText(L"00");
				m_tNPCScoreInfo.m_spTurn2_1Text->SetText(L"00");
			}

			m_spPlayerAnimation->SetStartEndIndex(0, 1);
			m_spNPCAnimation->SetStartEndIndex(1, 2);
		}
	}

	if (m_bPlaying)
	{
		CheckRound();

		if (!m_bFirst)
		{
			for (auto& iter : m_vecBowlingPin)
			{
				iter->SetLockFlags(0);
			}
			m_bFirst = true;
		}

		if (false == m_bFinish)
		{
			PerformTurn(_fDeltaSeconds);

			if (true == m_bNext)
			{
				m_fDelayTurnTime += _fDeltaSeconds;
				if (m_fDelayTurnTime > 1.7f)
				{
					NextTurn();
					m_fDelayTurnTime = 0.0f;
					m_bNext = false;
				}
			}

			DelayResetBowl(_fDeltaSeconds);
			DelayResetBowlingPin(_fDeltaSeconds);
		}
	}

	// ���� �� �̺�Ʈ�� �ֽ��ϴ�.
	if (true == m_bFinish)
	{
		m_spBowlingHUDPanel->SetActive(false);
		m_spAnglePanel->SetActive(false);
		m_spPositionPanel->SetActive(false);
		m_spPowerPanel->SetActive(false);

		// ���� ���Ḧ �ֽ��ϴ�.
		{
			m_spGamePanel->SetActive(true);
			if (true == IsVictory()) { m_spGameAnimation->SetTextureFileName(m_wstrGameSuccess); }
			else { m_spGameAnimation->SetTextureFileName(m_wstrGameFail); }
		}

		m_wpSoundManager.lock()->StopChannelEx(EChannelType::BACKGROUND_00);

		if (m_spGameAnimation->IsFinish())
		{
			//// ���⼭ �ð��� �Ǻ��� ���� bgm�� Ʋ���ݴϴ�.
			//m_wpSoundManager.lock()->StopChannelEx(EChannelType::BACKGROUND_00);
			//m_wpSoundManager.lock()->PlayBGM("");

			CGameManager::GetInstance()->GetPlayer()->SetActive(true);
			CGameManager::GetInstance()->GetPlayer()->SetOnAir();

			m_wpUIManager.lock()->GetHUDParentPanel()->SetActive(true);
			m_wpUIManager.lock()->FindParentsPanel(L"RidingUI")->SetActive(true);

			m_spGameAnimation->ResetFrame();
			m_spGamePanel->SetActive(false);
			m_bFinish = false;
			m_bPlaying = false;
			m_bStartEvent = false;
			m_bFirst = false;

			// npc�� ����Ŭ�� �ٲ��ݴϴ�.
			if (true == IsVictory()) { m_wpNPC.lock()->ChangeCycle(L"Cycle_Success"); }
			else { m_wpNPC.lock()->ChangeCycle(L"Cycle_Fail"); }
		}
	}

	return 0;
}

int32 Client::CBowlingSystem::PostTick(float _fDeltaSeconds)
{
	return 0;
}

HRESULT Client::CBowlingSystem::EndPlay()
{
	return S_OK;
}

void Client::CBowlingSystem::Release()
{
	m_spBowl = nullptr;

	for (auto& iter : m_vecBowlingPin)
	{
		iter = nullptr;
	}
	m_vecBowlingPin.clear();

	for (auto& iter : m_vecBowlingTriggers)
	{
		iter = nullptr;
	}
	m_vecBowlingTriggers.clear();

	m_vecBowlingPinPos.clear();

	m_spFloorObject = nullptr;

	// ui���� �ʱ�ȭ���ݴϴ�.
	m_spBowlingHUDPanel = nullptr;

	m_spPlayerAnimation = nullptr;
	m_spNPCAnimation = nullptr;

	m_spPositionImage = nullptr;
	m_spRoundImage = nullptr;

	m_spPowerPanel = nullptr;
	m_spPowerBarImage = nullptr;

	m_spAnglePanel = nullptr;
	m_spAngleImage = nullptr;

	// ����ü�ȿ� ���� ä���ݴϴ�.
	{
		m_tPlayerScoreInfo.m_spTurn0_0Text = nullptr;
		m_tPlayerScoreInfo.m_spTurn0_1Text = nullptr;

		m_tPlayerScoreInfo.m_spTurn1_0Text = nullptr;
		m_tPlayerScoreInfo.m_spTurn1_1Text = nullptr;

		m_tPlayerScoreInfo.m_spTurn2_0Text = nullptr;
		m_tPlayerScoreInfo.m_spTurn2_1Text = nullptr;
	}

	{
		m_tNPCScoreInfo.m_spTurn0_0Text = nullptr;
		m_tNPCScoreInfo.m_spTurn0_1Text = nullptr;

		m_tNPCScoreInfo.m_spTurn1_0Text = nullptr;
		m_tNPCScoreInfo.m_spTurn1_1Text = nullptr;

		m_tNPCScoreInfo.m_spTurn2_0Text = nullptr;
		m_tNPCScoreInfo.m_spTurn2_1Text = nullptr;
	}

	m_spBowlingCamera = nullptr;
}

void Client::CBowlingSystem::InitCamera()
{
	//FCameraDesc tCameraDesc;
	//// ������ ����� ��ġ�ϰ��մϴ�.
	//tCameraDesc.m_vInitPosition = Vector3(m_spBowl->GetPosition().value());
	//tCameraDesc.m_bUseMouseInput = false;
	//tCameraDesc.m_bUseKeyInput = false;

	//m_spBowlingCamera = CCamera::Create(make_shared<FCameraDesc>(tCameraDesc));

	FBowlingCameraDesc tCameraDesc;
	// ������ ����� ��ġ�ϰ��մϴ�.
	tCameraDesc.m_vInitPosition = Vector3(m_spBowl->GetPosition().value() - Vector3(0.f, -5.f, 5.f));
	tCameraDesc.m_bUseMouseInput = false;
	tCameraDesc.m_bUseKeyInput = false;

	m_spBowlingCamera = CBowlingCamera::Create(make_shared<FBowlingCameraDesc>(tCameraDesc));

	// �������� �ٶ󺸰� �մϴ�.
	//m_spBowlingCamera->LookAt(Vector3(0.f, -1.f, 0.f));
	m_spBowlingCamera->LookAt(m_spBowl->GetPosition().value());

	CWorld::GetInstance()->GetPostFrontLevel()->AddGameObject(m_spBowlingCamera);
}

void Client::CBowlingSystem::InitApearParticle()
{
	FBowlingPinAppearParticleDesc tBowlingPinAppearParticleDesc;

	tBowlingPinAppearParticleDesc.m_bActive = false;
	tBowlingPinAppearParticleDesc.m_wstrEffectGroupName = L"Group_BowlingPin_Appear";

	for (uint16 i = 0; i < 11; ++i)
	{
		/* JSON���Ϸκ��� ��ü�� �����մϴ�. */
		shared_ptr<CBowlingPinAppearParticle> spBowlingPinAppearParticle = CBowlingPinAppearParticle::CreateFromJson(make_shared<FBowlingPinAppearParticleDesc>(tBowlingPinAppearParticleDesc), tBowlingPinAppearParticleDesc.m_wstrEffectGroupName);
		CHECKF(spBowlingPinAppearParticle, L"Failed to CreateFromJson : CEffectManager");

		m_arrBowlingPinAppearEffects[i] = spBowlingPinAppearParticle;
	}

	shared_ptr<CLevel> spPostFrontLevel = CWorld::GetInstance()->GetPostFrontLevel();

	for (auto& bowlingpineffect : m_arrBowlingPinAppearEffects)
	{
		spPostFrontLevel->AddGameObject(bowlingpineffect);
	}
}

void Client::CBowlingSystem::PerformTurn(float _fDeltaSeconds)
{
		// �ƹ��͵� �������� ���� ��� ��ġ ���ϱ⸦ �����մϴ�.
		if (0 == m_iSelectNum)
		{
			SelectPos(_fDeltaSeconds);
		}
		// ��ġ�� ������ ��� ���� ���ϱ⸦ �����մϴ�.
		else if (1 == m_iSelectNum)
		{
			SelectAngle(_fDeltaSeconds);
		}
		// ��ġ�� ������ ������ ��� ���� ���ϱ⸦ �����մϴ�.
		else if (2 == m_iSelectNum)
		{
			SelectPower(_fDeltaSeconds);
		}
		// ��� ���������� �������� �����ϴ�.
		else 
		{
			if (false == m_spBowl->IsCulled())
			{
				// ó�� �������� �����Ҷ� �Ҹ��� ����մϴ�.
				if (false == m_bRollSound)
				{
					m_wpSoundManager.lock()->PlaySoundEx("BowlSound");
					m_bRollSound = true;
				}

				m_spBowl->ChangeAnimation((int32)ESheepBall::AS_SheepBall_Korogaru_Loop);
				m_spBowl->AddForce(m_spBowl->GetForwardVector().value(), m_fPower * 3.8f);

				// ī�޶� ���η��� ���󰡰� �մϴ�.
				if ((m_spBowl->GetPosition().value().x - 1.5f) < m_vBowlingLookAt.x)
				{
					m_spBowlingCamera->SetPosition(m_spBowl->GetPosition().value() - Vector3(4.5f, -2.0f, 0.f));
					m_spBowlingCamera->LookAt(m_spBowl->GetPosition().value());
				}
			}
		}
}

void Client::CBowlingSystem::SelectPos(float _fDeltaSeconds)
{
	//�������� �翷���� �������ݴϴ�.
	Vector3 vCurPos;

	if (m_bLeft)
	{
		//m_spBowl->MoveForward(2.f);
		m_spBowl->MoveLeft(2.f);
		if (m_vStartPos.z < m_spBowl->GetPosition().value().z) { m_bLeft = false; }
	}
	else
	{
		//m_spBowl->MoveBackward(2.f);
		m_spBowl->MoveRight(2.f);
		if (m_vEndPos.z > m_spBowl->GetPosition().value().z) { m_bLeft = true; }
	}

	m_spPositionPanel->SetActive(true);
	m_spPositionPanel->SetPivotWorldPoint(m_spBowl->GetPosition().value());

	if (ETurnType::PLAYER == m_eTurnType)
	{
		if (m_wpInputManager.lock()->IsMouseDown(EMouseActionType::LEFT_BUTTON))
		{
			m_vPos = m_spBowl->GetPosition().value();
			m_iSelectNum++;
			m_spPositionPanel->SetActive(false);
			m_spAnglePanel->SetActive(true);

			//���� ���带 ����մϴ�.
			m_wpSoundManager.lock()->PlaySoundEx("SelectSound");
		}
	}
	else if (ETurnType::AI == m_eTurnType)
	{
		m_fAISelectTime += _fDeltaSeconds;

		// AI�� ��� �ð��� ������ �� �������� �����մϴ�.
		if (m_fAISelectTime > 4.0f)
		{
			m_vPos = m_spBowl->GetPosition().value();
			m_iSelectNum++;
			m_fAISelectTime = 0.0f;
			m_spPositionPanel->SetActive(false);
			m_spAnglePanel->SetActive(true);

			//���� ���带 ����մϴ�.
			m_wpSoundManager.lock()->PlaySoundEx("SelectSound");
		}
	}
}

void Client::CBowlingSystem::SelectAngle(float _fDeltaSeconds)
{
	// �������� ȸ�������ݴϴ�.
	float fRadian = m_spBowl->GetQuaternion().value().ToEuler().y;
	float fDegree = XMConvertToDegrees(fRadian);

	if (m_bAngleLeft)
	{
		m_spBowl->Turn(Vector3(0.0f, 1.0f, 0.0f), 1.0f);
		if (fDegree > 110.0f) { m_bAngleLeft = false; }
	}
	else
	{
		m_spBowl->Turn(Vector3(0.0f, 1.0f, 0.0f), -1.0f);
		if (fDegree < 70.0f) { m_bAngleLeft = true; }
	}

	m_spBowl->SetPosition(m_vPos);
	m_spAnglePanel->SetPivotWorldPoint(m_spBowl->GetPosition().value());
	m_spAngleImage->SetRectAngle(-(fDegree - 90.f));

	if (ETurnType::PLAYER == m_eTurnType)
	{
		if (m_wpInputManager.lock()->IsMouseDown(EMouseActionType::LEFT_BUTTON))
		{
			m_fAngle = fDegree;
			m_iSelectNum++;
			m_spAnglePanel->SetActive(false);
			m_spPowerPanel->SetActive(true);

			//���� ���带 ����մϴ�.
			m_wpSoundManager.lock()->PlaySoundEx("SelectSound");
		}
	}
	else if (ETurnType::AI == m_eTurnType)
	{
		m_fAISelectTime += _fDeltaSeconds;

		if (m_fAISelectTime < 0.5f)
		{
			m_fAIAngle = CMathUtility::GetRandomFloat(80.f, 100.f);
		}

		// AI�� ��� �ð��� ������ �� �������� �����մϴ�.
		if (m_fAISelectTime > 4.0f)
		{
			if (m_fAIAngle >= fDegree - 2.f && m_fAIAngle < fDegree + 2.f)
			{
				m_fAngle = fDegree;
				m_iSelectNum++;
				m_fAISelectTime = 0.0f;
				m_spAnglePanel->SetActive(false);
				m_spPowerPanel->SetActive(true);

				//���� ���带 ����մϴ�.
				m_wpSoundManager.lock()->PlaySoundEx("SelectSound");
			}
		}
	}
}

void Client::CBowlingSystem::SelectPower(float _fDeltaSeconds)
{
	// �������� ������ ���⸦ �����մϴ�.
	if (m_bUp)
	{
		m_fPower += 0.05f;
		if (m_fPower > 7.0f) { m_bUp = false; }
	}
	else
	{
		m_fPower -= 0.05f;
		if (m_fPower <= 4.0f) { m_bUp = true; }
	}

	m_spBowl->SetPosition(m_vPos);
	float fCenterY = (1.f - ((m_fPower - 4.0f) / (7.0f - 4.0f))) * (48.f - (-40.f)) + (-40.f);

	m_spPowerPanel->SetPivotWorldPoint(m_spBowl->GetPosition().value() + Vector3(0.f, 0.35f, 0.f));
	m_spPowerBarImage->SetScreenRelativeCenter(Vector2(m_spPowerBarImage->GetScreenRelativeCenter().x, fCenterY));

	if (ETurnType::PLAYER == m_eTurnType)
	{
		if (m_wpInputManager.lock()->IsMouseDown(EMouseActionType::LEFT_BUTTON)) 
		{
			m_iSelectNum++;
			m_spPowerPanel->SetActive(false);

			//���� ���带 ����մϴ�.
			m_wpSoundManager.lock()->PlaySoundEx("SelectSound");
		}
	}
	else if (ETurnType::AI == m_eTurnType)
	{
		m_fAISelectTime += _fDeltaSeconds;
		// AI�� ��� �ð��� ������ �� �������� �����մϴ�.
		if (m_fAISelectTime > 4.0f)
		{
			m_iSelectNum++;
			m_fAISelectTime = 0.0f;
			m_spPowerPanel->SetActive(false);

			//���� ���带 ����մϴ�.
			m_wpSoundManager.lock()->PlaySoundEx("SelectSound");
		}
	}
}

void Client::CBowlingSystem::ResetBowl()
{
	m_spBowl->ChangeAnimation((int32)ESheepBall::AS_SheepBall_Shield);
	//m_spBowl->SetPosition(Vector3(-3373.5f, -9.0f, -2312.5f));
	m_fAngle = 0.0f;
	m_fPower = 4.0f;

	// ������ �ʱ�ȭ�����ݴϴ�.
	m_spBowl->RequestTransform(Vector3(-3396.3f, 10.3f, -2493.5f), Vector3(0.0f, 90.f, 0.0f));



	m_bDelayBowl = true;
	m_bRollSound = false;
}

void Client::CBowlingSystem::ResetBowlingPin()
{
	// ��ġ�� �ʱ�ȭ �����ݴϴ�. // ���� � �ʱ�ȭ ���������
	for (int32 i = 0; i < m_vecBowlingPin.size(); i++)
	{
		ActiveBowlingPinParticle(m_vecBowlingPin[i], m_vecBowlingPin[i]->GetMeshComponent()->GetModel());

		// �ʱ�ȭ�� lock�� �ɾ��ݴϴ�.
		m_vecBowlingPin[i]->SetLockFlags((int32)EAxisLockType::LOCK_ANGULAR_X | (int32)EAxisLockType::LOCK_ANGULAR_Y | (int32)EAxisLockType::LOCK_ANGULAR_Z);
		m_vecBowlingPin[i]->RequestTransform(m_vecBowlingPinPos[i], Vector3(0.f, 90.f, 0.f));
		m_vecBowlingPin[i]->SetCulled(true);
	}

	// �浹 ó���� �ʱ�ȭ �����ݴϴ�.
	for (auto& iter : m_vecBowlingPin)
	{
		iter->SetColl(false);
	}

	m_bDelayBowlingPin = true;
}

void Client::CBowlingSystem::DelayResetBowl(float _fDeltaSeconds)
{
	if (m_bDelayBowl)
	{
		m_fDelayBowlTime += _fDeltaSeconds;
		if (m_fDelayBowlTime > m_fLimitTime)
		{
			m_iSelectNum = 0;
			m_spBowl->SetCulled(false);
			m_bDelayBowl = false; 
			m_fDelayBowlTime = 0.0f;

			// ī�޶� ��ġ�� ���½����ݴϴ�.
			m_spBowlingCamera->SetPosition(m_vBowlingCameraPos);
			m_spBowlingCamera->LookAt(m_vBowlingLookAt);
		}
	}
}

void Client::CBowlingSystem::DelayResetBowlingPin(float _fDeltaSeconds)
{
	if (m_bDelayBowlingPin)
	{
		m_fDelayBowlingPinTime += _fDeltaSeconds;

		if (m_fDelayBowlingPinTime > m_fLimitTime)
		{ 
			// ���� ���� ���������ݴϴ�.
			{
				if (0 == m_iRoundCount || 1 == m_iRoundCount)
				{
					m_spRoundImage->SetTextureFileName(L"T_MiniGame_Round1Image");
				}
				else if (2 == m_iRoundCount || 3 == m_iRoundCount)
				{
					m_spRoundImage->SetTextureFileName(L"T_MiniGame_Round2Image");
				}
				else if (4 == m_iRoundCount || 5 == m_iRoundCount)
				{
					m_spRoundImage->SetTextureFileName(L"T_MiniGame_Round3Image");
				}
			}

			// �������� ���½����ݴϴ�.
			// �������� lock�� �������ݴϴ�.
			for (auto iter : m_vecBowlingPin)
			{
				iter->SetCulled(false);
				iter->SetLockFlags(0);
			}

			if (ETurnType::PLAYER == m_eTurnType)
			{
				if (3 >= m_iCurRoundNum)
				{
					m_wpSoundManager.lock()->PlaySoundEx("TurnChangeSound");
				}
			
				m_spPlayerAnimation->SetStartEndIndex(0, 1);
				m_spNPCAnimation->SetStartEndIndex(1, 2);

				m_iCurRoundNum++;
			}
			else if (ETurnType::AI == m_eTurnType)
			{
				m_wpSoundManager.lock()->PlaySoundEx("TurnChangeSound");
				m_spPlayerAnimation->SetStartEndIndex(1, 2);
				m_spNPCAnimation->SetStartEndIndex(0, 1);
			}

			m_fDelayBowlingPinTime = 0.0f;
			m_bDelayBowlingPin = false;
		}
	}
}

void Client::CBowlingSystem::NextTurn()
{
	// �������� ������ Ȯ���մϴ�.
	// �� ���� Ƚ���� �������� 10���� �� �Ѿ�� ��� ���� ���濡�� �ѱ�ϴ�.
	if (m_iBowlingPinNum >= 10 && m_iCurTurnNum < 1)
	{
		m_iCurTurnNum = 0;
		m_iRoundCount++;

		CheckBowlingPin();
		m_iBowlingPinTurn1Num = m_iBowlingPinNum;
		m_iBowlingPinTurn2Num = 0;

		if (ETurnType::PLAYER == m_eTurnType)
		{
			m_iPlayerTurnCount++;
			SaveTurnScore(m_eTurnType);
			m_iPlayerTurnCount++;
			SaveTurnScore(m_eTurnType);
			m_iPlayerSuccessNum += m_iBowlingPinNum;                                                         
			m_eTurnType = ETurnType::AI;
		}
		else if (ETurnType::AI == m_eTurnType)
		{
			m_iNPCTurnCount++;
			SaveTurnScore(m_eTurnType);
			m_iNPCTurnCount++;
			SaveTurnScore(m_eTurnType);
			m_iAISuccessNum += m_iBowlingPinNum;
			m_eTurnType = ETurnType::PLAYER;
		}

		ResetBowl();
		ResetBowlingPin();
	}
	// �������� 10���� �� �ȳѾ�� �� ���� Ƚ���� �����ִٸ� ���� ���½����ݴϴ�.
	// ��, �� ��° ���϶�
	else if (m_iBowlingPinNum < 10 && m_iCurTurnNum < 1)
	{
		m_iCurTurnNum++;
		CheckBowlingPin();
		m_iBowlingPinTurn1Num = m_iBowlingPinNum;
		// �Ѿ ������ ������ ���� ������ �����ݴϴ�.
		if (ETurnType::PLAYER == m_eTurnType)
		{
			m_iPlayerTurnCount++;
			SaveTurnScore(m_eTurnType);
			m_iPlayerSuccessNum += m_iBowlingPinTurn1Num;
		}
		else if (ETurnType::AI == m_eTurnType)
		{
			m_iNPCTurnCount++;
			SaveTurnScore(m_eTurnType);
			m_iAISuccessNum += m_iBowlingPinTurn1Num;
		}

		ResetBowl();
	}
	// ���� ������ Ƚ���� �����Ǿ��ٸ� ���� ���濡�� �ѱ�ϴ�.
	else if (1 <= m_iCurTurnNum)
	{
		m_iCurTurnNum = 0;
		m_iRoundCount++;

		CheckBowlingPin();
		m_iBowlingPinTurn2Num = m_iBowlingPinNum - m_iBowlingPinTurn1Num;

		if (ETurnType::PLAYER == m_eTurnType)
		{
			m_iPlayerTurnCount++;
			SaveTurnScore(m_eTurnType);
			m_iPlayerSuccessNum += m_iBowlingPinTurn2Num;
			m_eTurnType = ETurnType::AI;
			m_iBowlingPinNum = 0;
		}
		else if (ETurnType::AI == m_eTurnType)
		{
			m_iNPCTurnCount++;
			SaveTurnScore(m_eTurnType);
			m_iAISuccessNum += m_iBowlingPinTurn2Num;
			m_eTurnType = ETurnType::PLAYER;
			m_iBowlingPinNum = 0;
		}

		ResetBowl();
		ResetBowlingPin();
	}
}

void Client::CBowlingSystem::CheckRound()
{
	if (m_iCurRoundNum >= m_iMaxRoundNum)
	{
		m_bFinish = true;
		m_bPlaying = false;
	}
}

void Client::CBowlingSystem::CheckBowlingPin()
{
	m_iBowlingPinNum = 0;
	for (auto& iter : m_vecBowlingPin)
	{
		if (true == iter->IsColl())
		{
			m_iBowlingPinNum++;
		}
	}
}

void Client::CBowlingSystem::AddPanels()
{	
	m_spBowlingHUDPanel = m_wpUIManager.lock()->FindParentsPanel(L"BowlingUI");
	
	m_spPlayerAnimation = m_spBowlingHUDPanel->FindComponent<CAnimationImageComponent>(L"PlayerAnimation");
	m_spNPCAnimation = m_spBowlingHUDPanel->FindComponent<CAnimationImageComponent>(L"NPCAnimation");

	m_spRoundImage = m_spBowlingHUDPanel->FindComponent<CImageComponent>(L"RoundImage");

	m_spPowerPanel = m_wpUIManager.lock()->FindParentsPanel(L"BowlingPowerUI");
	m_spPowerBarImage = m_spPowerPanel->FindComponent<CImageComponent>(L"PowerBarImage");

	m_spAnglePanel = m_wpUIManager.lock()->FindParentsPanel(L"BowlingAngleUI");
	m_spAngleImage = m_spAnglePanel->FindComponent<CImageComponent>(L"AngleImage");

	m_spPositionPanel = m_wpUIManager.lock()->FindParentsPanel(L"BowlingPositionUI");
	m_spPositionImage = m_spPositionPanel->FindComponent<CImageComponent>(L"PositionImage");

	m_spGamePanel = m_wpUIManager.lock()->FindParentsPanel(L"BowlingGameStartUI");
	m_spGameAnimation = m_spGamePanel->FindComponent<CAnimationImageComponent>(L"Animation");

	// ����ü�ȿ� ���� ä���ݴϴ�.
	{
		m_tPlayerScoreInfo.m_spTurn0_0Text = m_spBowlingHUDPanel->FindComponent<CTextComponent>(L"Turn0_0Text");
		m_tPlayerScoreInfo.m_spTurn0_1Text = m_spBowlingHUDPanel->FindComponent<CTextComponent>(L"Turn0_1Text");

		m_tPlayerScoreInfo.m_spTurn1_0Text = m_spBowlingHUDPanel->FindComponent<CTextComponent>(L"Turn1_0Text");
		m_tPlayerScoreInfo.m_spTurn1_1Text = m_spBowlingHUDPanel->FindComponent<CTextComponent>(L"Turn1_1Text");
		
		m_tPlayerScoreInfo.m_spTurn2_0Text = m_spBowlingHUDPanel->FindComponent<CTextComponent>(L"Turn2_0Text");
		m_tPlayerScoreInfo.m_spTurn2_1Text = m_spBowlingHUDPanel->FindComponent<CTextComponent>(L"Turn2_1Text");
	}

	{
		m_tNPCScoreInfo.m_spTurn0_0Text = m_spBowlingHUDPanel->FindComponent<CTextComponent>(L"NPCTurn0_0Text");
		m_tNPCScoreInfo.m_spTurn0_1Text = m_spBowlingHUDPanel->FindComponent<CTextComponent>(L"NPCTurn0_1Text");

		m_tNPCScoreInfo.m_spTurn1_0Text = m_spBowlingHUDPanel->FindComponent<CTextComponent>(L"NPCTurn1_0Text");
		m_tNPCScoreInfo.m_spTurn1_1Text = m_spBowlingHUDPanel->FindComponent<CTextComponent>(L"NPCTurn1_1Text");

		m_tNPCScoreInfo.m_spTurn2_0Text = m_spBowlingHUDPanel->FindComponent<CTextComponent>(L"NPCTurn2_0Text");
		m_tNPCScoreInfo.m_spTurn2_1Text = m_spBowlingHUDPanel->FindComponent<CTextComponent>(L"NPCTurn2_1Text");
	}
}

void Client::CBowlingSystem::SaveTurnScore(ETurnType _eTurnType)
{
	//if (0 == m_iTurnCount)
	//{
	//
	//}

	std::wstring wstrScoreText;
	// player�� ���� ����մϴ�.
	if(ETurnType::PLAYER == _eTurnType)
	{
		if (1 == m_iPlayerTurnCount)
		{
			if (10 <= m_iBowlingPinTurn1Num)
			{
				wstrScoreText = to_wstring(m_iBowlingPinTurn1Num);
			}
			else
			{
				wstrScoreText = L"0" + to_wstring(m_iBowlingPinTurn1Num);
			}
			m_tPlayerScoreInfo.m_spTurn0_0Text->SetText(wstrScoreText);
		}
		else if (2 == m_iPlayerTurnCount)
		{
			if (10 <= m_iBowlingPinTurn2Num)
			{
				wstrScoreText = to_wstring(m_iBowlingPinTurn2Num);
			}
			else
			{
				wstrScoreText = L"0" + to_wstring(m_iBowlingPinTurn2Num);
			}
			m_tPlayerScoreInfo.m_spTurn0_1Text->SetText(wstrScoreText);
		}
		else if (3 == m_iPlayerTurnCount)
		{
			if (10 <= m_iBowlingPinTurn1Num)
			{
				wstrScoreText = to_wstring(m_iBowlingPinTurn1Num);
			}
			else
			{
				wstrScoreText = L"0" + to_wstring(m_iBowlingPinTurn1Num);
			}
			m_tPlayerScoreInfo.m_spTurn1_0Text->SetText(wstrScoreText);
		}
		else if (4 == m_iPlayerTurnCount)
		{
			if (10 <= m_iBowlingPinTurn2Num)
			{
				wstrScoreText = to_wstring(m_iBowlingPinTurn2Num);
			}
			else
			{
				wstrScoreText = L"0" + to_wstring(m_iBowlingPinTurn2Num);
			}
			m_tPlayerScoreInfo.m_spTurn1_1Text->SetText(wstrScoreText);
		}
		else if (5 == m_iPlayerTurnCount)
		{
			if (10 <= m_iBowlingPinTurn1Num)
			{
				wstrScoreText = to_wstring(m_iBowlingPinTurn1Num);
			}
			else
			{
				wstrScoreText = L"0" + to_wstring(m_iBowlingPinTurn1Num);
			}
			m_tPlayerScoreInfo.m_spTurn2_0Text->SetText(wstrScoreText);
		}
		else if (6 == m_iPlayerTurnCount)
		{
			if (10 <= m_iBowlingPinTurn2Num)
			{
				wstrScoreText = to_wstring(m_iBowlingPinTurn2Num);
			}
			else
			{
				wstrScoreText = L"0" + to_wstring(m_iBowlingPinTurn2Num);
			}
			m_tPlayerScoreInfo.m_spTurn2_1Text->SetText(wstrScoreText);
		}
	}

	// npc�� ���� ����մϴ�.
	else if(ETurnType::AI == _eTurnType)
	{
		if (1 == m_iNPCTurnCount)
		{
			if (10 <= m_iBowlingPinTurn1Num)
			{
				wstrScoreText = to_wstring(m_iBowlingPinTurn1Num);
			}
			else
			{
				wstrScoreText = L"0" + to_wstring(m_iBowlingPinTurn1Num);
			}
			m_tNPCScoreInfo.m_spTurn0_0Text->SetText(wstrScoreText);
		}
		else if (2 == m_iNPCTurnCount)
		{
			if (10 <= m_iBowlingPinTurn2Num)
			{
				wstrScoreText = to_wstring(m_iBowlingPinTurn2Num);
			}
			else
			{
				wstrScoreText = L"0" + to_wstring(m_iBowlingPinTurn2Num);
			}
			m_tNPCScoreInfo.m_spTurn0_1Text->SetText(wstrScoreText);
		}
		else if (3 == m_iNPCTurnCount)
		{
			if (10 <= m_iBowlingPinTurn1Num)
			{
				wstrScoreText = to_wstring(m_iBowlingPinTurn1Num);
			}
			else
			{
				wstrScoreText = L"0" + to_wstring(m_iBowlingPinTurn1Num);
			}
			m_tNPCScoreInfo.m_spTurn1_0Text->SetText(wstrScoreText);
		}
		else if (4 == m_iNPCTurnCount)
		{
			if (10 <= m_iBowlingPinTurn2Num)
			{
				wstrScoreText = to_wstring(m_iBowlingPinTurn2Num);
			}
			else
			{
				wstrScoreText = L"0" + to_wstring(m_iBowlingPinTurn2Num);
			}
			m_tNPCScoreInfo.m_spTurn1_1Text->SetText(wstrScoreText);
		}
		else if (5 == m_iNPCTurnCount)
		{
			if (10 <= m_iBowlingPinTurn1Num)
			{
				wstrScoreText = to_wstring(m_iBowlingPinTurn1Num);
			}
			else
			{
				wstrScoreText = L"0" + to_wstring(m_iBowlingPinTurn1Num);
			}
			m_tNPCScoreInfo.m_spTurn2_0Text->SetText(wstrScoreText);
		}
		else if (6 == m_iNPCTurnCount)
		{
			if (10 <= m_iBowlingPinTurn2Num)
			{
				wstrScoreText = to_wstring(m_iBowlingPinTurn2Num);
			}
			else
			{
				wstrScoreText = L"0" + to_wstring(m_iBowlingPinTurn2Num);
			}
			m_tNPCScoreInfo.m_spTurn2_1Text->SetText(wstrScoreText);
		}
	}
}

void Client::CBowlingSystem::SetGameStart()
{
	m_spGameAnimation->SetTextureFileName(m_wstrGameStart);
	m_bStartEvent = true;
}

shared_ptr<CBowlingPinAppearParticle> Client::CBowlingSystem::ActiveBowlingPinParticle(std::shared_ptr<CGameObject> _spGameObject, std::shared_ptr<CModel> _spModel)
{
	for (auto& bowlingpineffect : m_arrBowlingPinAppearEffects)
	{
		if (!bowlingpineffect->IsActive())
		{
			bowlingpineffect->SetPosition(_spGameObject->GetPosition().value());
			bowlingpineffect->ResetAtBonePosition(_spGameObject, _spModel);
			return bowlingpineffect;
		}
	}

	return nullptr;
}