#include "pch.h"
#include "PalSphere.h"

// engine manager
#include "EngineModule.h"
#include "CameraManager.h"
#include "EffectManager.h"

// client manager
#include "UIManager.h"
#include "GameManager.h"
#include "SoundManager.h"

// component
#include "CameraComponent.h"
#include "MeshComponent.h"
#include "SphereComponent.h"
#include "TrailComponent.h"
#include "ProgressBarComponent.h"
#include "ImageComponent.h"
#include "TextComponent.h"
#include "AnimationImageComponent.h"
#include "BehaviorTreeComponent.h"

// system
#include "PalBoxSystem.h"

// utility
#include "MathUtility.h"

// gameobject
#include "Player.h"
#include "Monster.h"
#include "EffectGroup.h"

// level
#include "Level.h"
#include "Model.h"
#include "Panel.h"
#include "Bone.h"

std::shared_ptr<CPalSphere> Client::CPalSphere::Create(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	shared_ptr<CPalSphere> spPalSphere = make_shared<CPalSphere>();
	ENSUREF(spPalSphere->Initialize(_spGameObjectDesc), L"Failed to initialize pal sphere.");
	return spPalSphere;
}

HRESULT Client::CPalSphere::Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	if (FAILED(CProjectile::Initialize(_spGameObjectDesc))) { return E_FAIL; }

	InitTrailComponent();

	// etc
	m_fLocalGravity = 25.0f;
	m_spMeshComponent->SetUseAnimation(false);
	
	m_wpUIManager = CUIManager::GetInstance();
	m_wpPalBoxSystem = CGameManager::GetInstance()->GetPalBoxSystem();
	m_wpSoundManager = CEngineModule::GetInstance()->GetSoundManager();

	return S_OK;
}

HRESULT Client::CPalSphere::BeginPlay()
{
	if (FAILED(CProjectile::BeginPlay())) { return E_FAIL; }

	return S_OK;
}

int32 Client::CPalSphere::PreTick(float _fDeltaSeconds)
{
	CProjectile::PreTick(_fDeltaSeconds);

	return 0;
}

int32 Client::CPalSphere::Tick(float _fDeltaSeconds)
{
	CProjectile::Tick(_fDeltaSeconds);

	// 팰 스피어가 야생 팰과 부딪힌 경우
	if (!m_bHit) { return 0; }
	
	// 팰 스피어와 UI 위치를 동기화시켜줍니다.
	if (m_spCapturePanel)
	{
		m_spCapturePanel->SetPivotWorldPoint(GetPosition().value());
	}

	
	switch (m_eState)
	{
	case Client::CPalSphere::EPalSphereState::OnThrow:
		break;

	case Client::CPalSphere::EPalSphereState::OnMoveToPosition:
	{
		Vector3 vPos = GetPosition().value();
		if ((vPos - m_vDstPos).Length() < 0.05f)
		{
			m_wpSoundManager.lock()->PlaySoundEx("AKE_Player_Sphere_CaptureJudge_Flash_145594866", GetPosition().value());
			m_eState = EPalSphereState::OnLooping;
		}
		else
		{
			//vPos = Vector3::Lerp(vPos, m_vDstPos, 0.1f);
			vPos = CMathUtility::EaseOutBack(vPos, m_vDstPos, 0.1f);
			SetPosition(vPos);
		}
	}
		break;
	case Client::CPalSphere::EPalSphereState::OnLooping:
	{
		// 진입시
		if (!m_bAnimationPlaying)
		{
			m_spMeshComponent->SetUseAnimation(true);
			m_spMeshComponent->ChangeAnimation((int32)EPALSPHERE::AS_PalSphereCapture, false, 0.8f);
			m_wpSoundManager.lock()->PlaySoundEx("AKE_Player_Sphere_CaptureJudge_Shake_51322031", GetPosition().value());

			m_bAnimationPlaying = true;
		}
		// 진입 후
		else
		{
			const FAnimationData& tAnimData = m_spMeshComponent->GetModel()->GetModelAnimationData().m_tBaseAnimData;
			
			// 루프 한번 끝낸 이후 반복로직
			if (tAnimData.m_iLoopCount > 0)
			{
				m_fTimeChecker += _fDeltaSeconds;

				// UI
				if (1.0f >= m_fTimeChecker)
				{
					// 포획 UI를 보간하고 연동해줍니다.
					float fPercentageRatio = CMathUtility::EaseInOutSine(static_cast<float>(m_vecCaptureProbability[m_iCurLoopNum]), static_cast<float>(m_vecCaptureProbability[m_iCurLoopNum + 1]), m_fTimeChecker);

					if (fPercentageRatio >= 99) { fPercentageRatio = 100.0f; }
					m_spCaptureText->SetText(to_wstring(static_cast<int32>(fPercentageRatio)));
					fPercentageRatio = fPercentageRatio * 0.01f;
					m_spProgress->SetRatio(fPercentageRatio);

					Vector4 vPreColor = m_wpUIManager.lock()->ChangeCirCleProgressBarColor(static_cast<float>(m_vecCaptureProbability[m_iCurLoopNum] * 0.01f));
					Vector4 vCurColor = m_wpUIManager.lock()->ChangeCirCleProgressBarColor(static_cast<float>(m_vecCaptureProbability[m_iCurLoopNum + 1] * 0.01f));

					// 색상을 보간합니다.
					Vector4 vChangeColor = Vector4::Lerp(vPreColor, vCurColor, m_fTimeChecker);
					m_spProgress->SetColor(vChangeColor);
					m_spArrowImage->SetColor(vChangeColor);
				}

				// 일정 시간 멈춰있었다면 다시 애니메이션 재생
				if (m_fTimeChecker > m_fShakeWaitTime)
				{
					m_spMeshComponent->GetModel()->SetStopAnimation(false);
				}
			
				if (m_bLoopEnded)
				{
					if (tAnimData.m_iLoopCount > m_iNumLoopCount)
					{
						// 야생 팰을 잡은 경우
						if (m_bSuccessCatch)
						{
							m_eState = EPalSphereState::OnSuccessCatch;
							m_wpEffectManager.lock()->ActivateEffect(L"Group_PalCapture_Successed", GetPosition().value());
							m_wpSoundManager.lock()->PlaySoundEx("AKE_Player_Sphere_CaptureSuccess_204022204", GetPosition().value());
						}
						// 야생 팰을 잡지 못한 경우
						else
						{
							// 팰을 팰 스피어 위치로 이동시킵니다.
							m_wpHitMonster.lock()->ModifyBoolValue(EBlackboardBoolValue::BOOL_ESCAPE, true);
							m_wpHitMonster.lock()->GetBodyColliderComponent()->ApplySimulationDirectly(true);
							m_wpHitMonster.lock()->SetJump(true);
							m_wpHitMonster.lock()->SetPosition(GetPosition().value());
							m_wpHitMonster.lock()->SetCulled(false);
							// m_wpHitMonster.lock()->SetActive(true);
							m_wpSoundManager.lock()->PlaySoundEx("AKE_Player_Sphere_CaptureFail_104962853", GetPosition().value());

							m_spMeshComponent->SetUseAnimation(false);
							ResetDataMember();
							SetCulled(false);
							SetActive(false);
						}
					}
					else
					{
						m_spMeshComponent->GetModel()->SetStopAnimation(true);
						m_wpSoundManager.lock()->PlaySoundEx("AKE_Player_Sphere_CaptureJudge_Shake_51322031", GetPosition().value());
						m_fTimeChecker = 0.f;
						m_iCurLoopNum++;
					}

					m_bLoopEnded = false;
				}

			}

			if (tAnimData.m_bLoopEnd)
			{
				m_bLoopEnded = true;
			}
		}
	}
	break;

	case Client::CPalSphere::EPalSphereState::OnSuccessCatch:
	{		
		SetCulled(true);
		m_spMeshComponent->SetUseAnimation(false);
		if (m_spCapturePanel) { m_spCapturePanel->SetActive(false); }
		if (m_spSuccessPanel)
		{
			m_spSuccessPanel->SetActive(true);
			m_spSuccessPanel->SetPivotWorldPoint(GetPosition().value());
		}

		m_fSuccessTimeChecker += _fDeltaSeconds;
		if (true == m_spAnimation->IsFinish())
		{
			if (m_wpOwnedPlayer.lock()->TryToAddCapturedPal(m_wpHitMonster.lock()))
			{
				m_wpHitMonster.lock()->SetAllyCollisionLayer();
				m_wpHitMonster.lock()->ReturnPalHPPanel();
				m_wpHitMonster.lock()->ReInitializePalBT(EMonsterAIType::PARTNER_PAL);
				m_wpHitMonster.lock()->SetPosition({ 0.0f, -100.0f, 0.0f });
				m_wpHitMonster.lock()->SetActive(false);
			}
			else
			{
				// 가운데 패널에 넣습니다.
				m_wpPalBoxSystem.lock()->AddPalToPalBoxMidPanel(m_wpHitMonster.lock());

				shared_ptr<CLevel> spLevel = m_wpHitMonster.lock()->GetOwnerLevel();
				spLevel->RemoveGameObject(m_wpHitMonster.lock());

				// TODO; 옥트리가 팰을 갖고 있기 때문에 옥트리에서도 삭제해야 한다.
			}

			m_wpSoundManager.lock()->PlaySoundEx("Capture_3_889693337", GetPosition().value());

			m_spMeshComponent->SetUseAnimation(false);
			ResetDataMember();
			SetCulled(false);
			SetActive(false);
		}
	}
	break;

	default:
		break;
	}
	
	return 0;
}

int32 Client::CPalSphere::PostTick(float _fDeltaSeconds)
{
	CProjectile::PostTick(_fDeltaSeconds);

	return 0;
}

HRESULT Client::CPalSphere::EndPlay()
{
	if (FAILED(CProjectile::EndPlay())) { return E_FAIL; }

	return S_OK;
}

void Client::CPalSphere::Release()
{
	m_vecCaptureProbability.clear();
	m_spCapturePanel = nullptr;
	m_spProgress = nullptr;
	m_spArrowImage = nullptr;
	m_spCaptureText = nullptr;

	CProjectile::Release();
}

void Client::CPalSphere::OnTriggerEnter(CComponent* _pSrcComponent, CComponent* _pDstComponent)
{
	if (!m_bTriggerEnterCalled)
	{
		// 팰과 부딪친 경우
		CColliderComponent* pSrcColliderComponent = dynamic_cast<CColliderComponent*>(_pSrcComponent);
		CColliderComponent* pDstColliderComponent = dynamic_cast<CColliderComponent*>(_pDstComponent);

		if (pSrcColliderComponent == m_wpCollider.lock().get() &&
			pDstColliderComponent->GetCollisionLayer() == ECollisionLayer::MONSTER)
		{
			m_wpSoundManager.lock()->PlaySoundEx("PAL_Sphere_02_Start_574309610", GetPosition().value());

			// 야생 팰과 부딪친 경우
			shared_ptr<CMonster> spHitMonster = dynamic_pointer_cast<CMonster>(pDstColliderComponent->GetOwnerGameObject());
			m_wpHitMonster = spHitMonster;

			m_wpEffectManager.lock()->ActivePalSummonParticle(spHitMonster, spHitMonster->GetMeshComponent()->GetModel());

			EMonsterAIType eAIType = spHitMonster->GetMonsterAIType();
			if (EMonsterAIType::WILD_PAL == eAIType)
			{
				Vector3 vUp = Vector3(0.0f, 1.0f, 0.0f);
				Vector3 vRight = CEngineModule::GetInstance()->GetCameraManager()->GetCurCameraWorldRightVector();
				Vector3 vLook = CEngineModule::GetInstance()->GetCameraManager()->GetCurCameraWorldForwardVector();

				m_bHit = true;
				m_bThrowing = false;
				m_eState = EPalSphereState::OnMoveToPosition;

				Vector3 vStartPos = GetPosition().value();
				m_vDstPos = vStartPos + vRight * m_vDstRelativePos.x + vUp * m_vDstRelativePos.y - vLook * m_vDstRelativePos.z;

				tuple<bool, int32> tpResult = spHitMonster->ComputeCaptureProbability();// 여기서 팰 잡기 성공하는 경우를 처리하자.
				auto& [bCaptureResult, iNumShakes] = tpResult;
				m_bSuccessCatch = bCaptureResult;
				m_iNumLoopCount = iNumShakes;

				// 포획 이펙트 재생
				m_wpEffectManager.lock()->ActivateEffect(L"Group_PalCapture", GetPosition().value());
				// m_wpEffectManager.lock()->ActivateEffect(L"Group_PalCapture_Catched", GetPosition().value());
				// m_wpEffectManager.lock()->ActivateEffect(L"Group_PalCapture_FirstLoop", GetPosition().value());

				// 포획 UI 가져오기 및 정보 연동
				m_spCapturePanel = m_wpUIManager.lock()->GetUsingCaptureUI();
				m_spCaptureText = m_spCapturePanel->FindChildGameObject(L"ChildProbabilityUI")->FindComponent<CTextComponent>(L"ProbabilityText");
				m_spArrowImage = m_spCapturePanel->FindChildGameObject(L"ChildProbabilityUI")->FindComponent<CImageComponent>(L"UpArrowImage");
				m_spProgress = m_spCapturePanel->FindComponent<CProgressBarComponent>(L"CircleProgressBar");
				m_spCapturePanel->SetActive(true);

				// 수치를 연동시켜줍니다.
				int32 iCapturePercent = spHitMonster->GetCapturePercentage();
				m_spCaptureText->SetText(to_wstring(iCapturePercent));
				m_spProgress->SetRatio(iCapturePercent * 0.01f);
				m_spArrowImage->SetColor(m_wpUIManager.lock()->ChangeCirCleProgressBarColor(iCapturePercent * 0.01f));
				m_spProgress->SetColor(m_wpUIManager.lock()->ChangeCirCleProgressBarColor(iCapturePercent * 0.01f));
				m_spCapturePanel->SetPivotWorldPoint(GetPosition().value());

				ComputeCaptureProbability(iCapturePercent);
				m_bTriggerEnterCalled = true;

				// 포획 성공 UI를 연동합니다.
				if (true == m_bSuccessCatch)
				{
					m_spSuccessPanel = m_wpUIManager.lock()->GetUsingCaptureSuccessUI();
					m_spAnimation = m_spSuccessPanel->FindComponent<CAnimationImageComponent>(L"SuccessAnimation0");
					m_spSuccessPanel->SetActive(false);
				}
			}
		}


		// 이펙트 비활성화
		if (!m_wpEffectGroup.expired())
		{
			m_wpEffectGroup.lock()->EndLifeTime();
		}
		if (m_spTrailComponent)
		{
			m_spTrailComponent->ClearRectList(GetPosition().value() + Vector3(0.f, m_fTrailSize, 0.f), GetPosition().value() + Vector3(0.f, -m_fTrailSize, 0.f));
		}
	}
}

void Client::CPalSphere::InitTrailComponent()
{
	FTrailComponentDesc tTrailComponentDesc;
	tTrailComponentDesc.m_wstrComponentName = L"TrailComponent";
	tTrailComponentDesc.m_eRenderType		= ERenderType::RENDER_GLOW;
	tTrailComponentDesc.m_eComponentType	= EComponentType::PARTICLE;
	tTrailComponentDesc.m_wpOwnerGameObject = shared_from_this();

	tTrailComponentDesc.m_iNumRects			= 20;
	tTrailComponentDesc.m_fMaxLifeTime		= 0.001f;
	tTrailComponentDesc.m_vColor			= Color(0.6f, 0.9f, 1.0f, 1.f);

	tTrailComponentDesc.m_wstrEffectShaderName = L"FX_Trail_PT2";

	tTrailComponentDesc.m_bUseDiffuse		= false;
	tTrailComponentDesc.m_bUseNoise			= false;
	tTrailComponentDesc.m_bUseMask			= true;
	tTrailComponentDesc.m_bUseDistortion	= false;
	tTrailComponentDesc.m_bUseDissolve		= false;

	tTrailComponentDesc.m_wstrDiffuseTextureName	= L"";
	tTrailComponentDesc.m_wstrNoiseTextureName		= L"";
	tTrailComponentDesc.m_wstrMaskTextureName		= L"T_VFX_tk_Barrier_deceive_01";
	tTrailComponentDesc.m_wstrDistortionTextureName = L"";
	tTrailComponentDesc.m_wstrDissolveTextureName	= L"";

	m_spTrailComponent = CTrailComponent::Create(make_shared<FTrailComponentDesc>(tTrailComponentDesc));
	CHECKF(m_spTrailComponent, L"Failed to Create TrailComponent : CProjectile");

	AddComponent(tTrailComponentDesc.m_wstrComponentName, m_spTrailComponent);
}

void Client::CPalSphere::ResetDataMember()
{
	CProjectile::ResetDataMember();

	m_bAnimationPlaying = false;
	m_bHit = false;
	m_vDstPos = Vector3(0.0f);
	m_iNumLoopCount = 0;
	m_bSuccessCatch = false;
	m_fTimeChecker = 0.0f;
	m_bLoopEnded = false;
	m_fSuccessTimeChecker = 0.f;
	m_eState = EPalSphereState::OnThrow;
	m_bTriggerEnterCalled = false;

	m_iCurLoopNum = 0;
	m_vecCaptureProbability.clear();
	if (nullptr != m_spCapturePanel)
	{
		m_wpUIManager.lock()->ReturnCaptureUI(m_spCapturePanel); 
	}
	m_spCapturePanel = nullptr;
	m_spProgress = nullptr;
	m_spArrowImage = nullptr;
	m_spCaptureText = nullptr;
	if (nullptr != m_spSuccessPanel)
	{
		m_wpUIManager.lock()->ReturnCaptureSuccessUI(m_spSuccessPanel);
	}
	m_spSuccessPanel = nullptr;
	m_spAnimation = nullptr;
}

void Client::CPalSphere::ComputeCaptureProbability(int32 _iCapturePercentage)
{
	// 남은 포획 확률
	int32 iRemainCapturePercentage =  100 - _iCapturePercentage;
	
	if (true == m_bSuccessCatch)
	{
		// 일정 확률
		int32 iRegularCapturePercentage = iRemainCapturePercentage / m_iNumLoopCount;

		for (int32 i = 0; i <= m_iNumLoopCount + 1; i++)
		{
			int32 iCapturePercentage = 0;
			if (0 == i)
			{
				iCapturePercentage = _iCapturePercentage;
			}
			else if (m_iNumLoopCount + 1 == i)
			{
				iCapturePercentage = 100;
			}
			else
			{
				iCapturePercentage = CMathUtility::GetRandomInt(m_vecCaptureProbability[i - 1], (_iCapturePercentage + iRegularCapturePercentage * i));
			}
			m_vecCaptureProbability.push_back(iCapturePercentage);
		}
	}

	else
	{
		int32 iRandomNum = CMathUtility::GetRandomInt(1, 5);
		// 일정 확률
		int32 iRegularCapturePercentage = iRemainCapturePercentage / (m_iNumLoopCount + iRandomNum);

		for (int32 iIndex = 0; iIndex <= m_iNumLoopCount + 1; iIndex++)
		{
			int32 iCapturePercentage = 0;
			if (0 == iIndex)
			{
				iCapturePercentage = _iCapturePercentage;
			}
			//else if (m_iNumLoopCount + 1 == i)
			//{
			//	//iCapturePercentage = 100;
			//}
			else
			{
				iCapturePercentage = CMathUtility::GetRandomInt(m_vecCaptureProbability[iIndex - 1], (_iCapturePercentage + iRegularCapturePercentage * iIndex));
			}
			m_vecCaptureProbability.push_back(iCapturePercentage);
		}
	}
}

void Client::CPalSphere::AnimNotifyCall(const list<FAnimNotifyData>& _lstAnimNotify)
{
	for (FAnimNotifyData tData : _lstAnimNotify)
	{
		switch (tData.m_eNotifyType)
		{
		case ENotifyType::SOUND:
			PlaySoundWithID(tData);
			break;

		case ENotifyType::EFFECT:
			PlayEffect(tData);
			break;

		default:
			break;
		}
	}
}

void Client::CPalSphere::PlaySoundWithID(const FAnimNotifyData& _tNotifyData)
{
}

void Client::CPalSphere::PlayEffect(const FAnimNotifyData& _tNotifyData)
{
	FEffectNotifyData tEffectNotifyData = _tNotifyData.m_tEffectNotify;
	shared_ptr<CBone> spBone = m_spMeshComponent->GetBonePtr(tEffectNotifyData.m_iAttachBoneIndex);

	if (tEffectNotifyData.m_bUseFollowBone)
	{
		m_wpEffectManager.lock()->ActivateEffectCombined(tEffectNotifyData.m_wstrEffectName, shared_from_this(), spBone, tEffectNotifyData.m_vRelativeLocation);
	}
	else
	{
		// 위치 조정
		Matrix offsetBoneMatrix = CMathUtility::GetScaledKilledMatrix(spBone->GetCombinedTransformationMatrix());
		Matrix relativeMatrix = Matrix::CreateTranslation(tEffectNotifyData.m_vRelativeLocation);
		Matrix matWorld = relativeMatrix * offsetBoneMatrix * GetTransformMatrix().value();
		m_wpEffectManager.lock()->ActivateEffect(tEffectNotifyData.m_wstrEffectName, shared_from_this(), matWorld.Translation());
	}
}