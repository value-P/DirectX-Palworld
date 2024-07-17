#include "pch.h"
#include "Nature.h"

#include "ClientLoop.h"

// module
#include "ClientLoop.h"
#include "EngineModule.h"
#include "ClientModule.h"

// manager
#include "GameManager.h"
#include "CameraManager.h"

// manager
#include "UIManager.h"

// system
#include "ToDoSystem.h"

// gameobject
#include "Panel.h"
#include "Camera.h"
#include "CameraComponent.h"

// component
#include "AnimationImageComponent.h"
#include "ColliderComponent.h"
 
// utility
#include "MathUtility.h"    

HRESULT Client::CNature::Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	// habitat desc
	if (FAILED(CHabitat::Initialize(_spGameObjectDesc))) { return E_FAIL; }

	m_wpSoundManager = CEngineModule::GetInstance()->GetSoundManager();

	// nature desc
	shared_ptr<FNatureDesc> spNatureDesc = static_pointer_cast<FNatureDesc>(_spGameObjectDesc);
	m_eNatureType = spNatureDesc->m_eNatureType;

	m_eHabitatType = EHabitatType::NATURE;
	
	m_wpGetItemParentPanel = CUIManager::GetInstance()->FindParentsPanel(L"GetItemPanel");
	m_wpGetItemParentPanel.lock()->SetActive(true);
	m_wpGetItemAnimationImageComponentSlot0 = m_wpGetItemParentPanel.lock()->FindComponent<CAnimationImageComponent>(L"get_item_slot_0");
	m_wpGetItemAnimationImageComponentSlot1 = m_wpGetItemParentPanel.lock()->FindComponent<CAnimationImageComponent>(L"get_item_slot_1");
	m_wpGetItemAnimationImageComponentSlot0.lock()->SetActive(false);
	m_wpGetItemAnimationImageComponentSlot1.lock()->SetActive(false);

	return S_OK;
}

HRESULT Client::CNature::BeginPlay()
{
	if (FAILED(CHabitat::BeginPlay())) { return E_FAIL; }

	return S_OK;
}

int32 Client::CNature::PreTick(float _fDeltaSeconds)
{
	switch(CHabitat::PreTick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}
	return 0;
}

int32 Client::CNature::Tick(float _fDeltaSeconds)
{
	switch(CHabitat::Tick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	RespawnNature(_fDeltaSeconds);
	return 0;
}

int32 Client::CNature::PostTick(float _fDeltaSeconds)
{
	switch(CHabitat::PostTick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}
	return 0;
}

HRESULT Client::CNature::EndPlay()
{
	if (FAILED(CHabitat::EndPlay())) { return E_FAIL; }

	return S_OK;
}

void Client::CNature::Release()
{
	CHabitat::Release();
}

void Client::CNature::TakeDamage(const std::shared_ptr<CGameObject>& _spInstigator, float _fDamage)
{
	return;
}

void Client::CNature::RespawnNature(float _fDeltaSeconds)
{
	return;
}

bool Client::CNature::DestroyNature(float _fDeltaSeconds)
{
	return false;
}

bool Client::CNature::IsDropItem()
{
	if (CMathUtility::GetRandomFloat(0.0f, 1.0f) < CClientLoop::s_spClientDataSheet->k_fItemDropPercentage)
	{
		return true;
	}
	
	return false;
}

void Client::CNature::ShakeNature(float _fDeltaSeconds)
{
	if (!m_bShaked) { return; }

	float fMaxShakeDeltaSeconds = CClientLoop::s_spClientDataSheet->k_fMaxShakeDeltaSeconds;
	weak_ptr<CCameraComponent> wpCameraComponent = CEngineModule::GetInstance()->GetCameraManager()->GetCurCamera();
	wpCameraComponent.lock()->ShakeCamera(EShakeType::SHAKE_RIGHT, 0.0f, 0.15f, fMaxShakeDeltaSeconds);

	m_fSumShakedDeltaSecond += _fDeltaSeconds;
	if (fMaxShakeDeltaSeconds <= m_fSumShakedDeltaSecond)
	{
		m_fSumShakedDeltaSecond = 0.0f;
		m_bShaked = false;
		m_bStartShaking = false;
	}
}
