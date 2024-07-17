#include "pch.h"
#include "PalInteractionSystem.h"

#include "SoundManager.h"

#include "UIManager.h"
#include "Panel.h"
#include "EngineModule.h"
#include "InputManager.h"
#include "ImageComponent.h"

std::shared_ptr<CPalInteractionSystem> Client::CPalInteractionSystem::Create(const std::shared_ptr<FPalInteractionSystemDesc>& _spPalInteractionSystemDesc)
{
	shared_ptr<CPalInteractionSystem> spInstance = make_shared<CPalInteractionSystem>();
	CHECKF(spInstance, L"CPalInteractionSystem : Create Failed");
	spInstance->Initialize(_spPalInteractionSystemDesc);
	return spInstance;
}

HRESULT Client::CPalInteractionSystem::Initialize(const std::shared_ptr<FPalInteractionSystemDesc>& _spPalInteractionSystemDesc)
{
	m_wpInputManager = CEngineModule::GetInstance()->GetInputManager();
	m_wpUIManager = CUIManager::GetInstance();
	m_wpSoundManager = CEngineModule::GetInstance()->GetSoundManager();

	m_spPalInteractionUI = m_wpUIManager.lock()->FindParentsPanel(L"PalInteractionUI");
	m_spHoverImage = m_spPalInteractionUI->FindComponent<CImageComponent>(L"HoverImage");
	m_spNameImage = m_spPalInteractionUI->FindComponent<CImageComponent>(L"NameImage");
	return S_OK;
}

HRESULT Client::CPalInteractionSystem::BeginPlay()
{
	AddvecIndex();

	return S_OK;
}

int32 Client::CPalInteractionSystem::PreTick(float _fDeltaSeconds)
{
	return 0;
}

int32 Client::CPalInteractionSystem::Tick(float _fDeltaSeconds)
{
	if (m_wpUIManager.lock()->GetTopPanel() == m_spPalInteractionUI)
	{
		for (auto& pair : m_vecIndex)
		{
			if (-1.0f < m_wpUIManager.lock()->GetCircleDegreeAngle())
			{
				if (IsInAngle(pair.second.fMinAngle, pair.second.fMaxAngle, m_wpUIManager.lock()->GetCircleDegreeAngle()))
				{
					m_iCurIndex = pair.first;
				}
			}
			else if (-1.0f == m_wpUIManager.lock()->GetCircleDegreeAngle())
			{
				m_iCurIndex = -1;
			}
		}

		if (-1 == m_iCurIndex)
		{
			m_spHoverImage->SetActive(false);
		}
		else
		{
			m_spHoverImage->SetActive(true);
			m_spHoverImage->SetRectAngle(-m_vecIndex[m_iCurIndex].second.fMinAngle);

			if (m_iPreIndex != m_iCurIndex)
			{
				m_wpSoundManager.lock()->PlaySoundEx("HoverSound");
				m_iPreIndex = m_iCurIndex;
			}
		}
	}



	return 0;
}

int32 Client::CPalInteractionSystem::PostTick(float _fDeltaSeconds)
{
	return 0;
}

HRESULT Client::CPalInteractionSystem::EndPlay()
{
	return S_OK;
}

void Client::CPalInteractionSystem::Release()
{
	m_spHoverImage = nullptr;
	m_spPalInteractionUI = nullptr;

	m_vecIndex.clear();
	m_vecPlayerIndex.clear();
}

bool Client::CPalInteractionSystem::IsInteracUIActivate()
{
	return m_spPalInteractionUI->IsActive();
}

void Client::CPalInteractionSystem::ToggleInteractUI()
{
	if (false == m_spPalInteractionUI->IsActive() && !m_wpUIManager.lock()->IsOnUIPanel())
	{
		m_eCurSystemUIType = EInteractionSystemUIType::EPalInteract;
		
		m_spNameImage->SetTextureFileName(m_wstrPalImageName);
		m_spPalInteractionUI->SetActive(true);
		m_wpUIManager.lock()->SetTopPanel(m_spPalInteractionUI);

		m_wpSoundManager.lock()->PlaySoundEx("KeyInputSound");
	}
	else if (true == m_spPalInteractionUI->IsActive())
	{
		m_spPalInteractionUI->SetActive(false);
		m_wpUIManager.lock()->ResetTopPanel();

		m_wpSoundManager.lock()->PlaySoundEx("KeyInputSound");
	}
}

void Client::CPalInteractionSystem::ToggleEmoteUI()
{
	if (false == m_spPalInteractionUI->IsActive() && !m_wpUIManager.lock()->IsOnUIPanel())
	{
		m_eCurSystemUIType = EInteractionSystemUIType::EEmote;

		m_spNameImage->SetTextureFileName(m_wstrPlayerImageName);
		m_spPalInteractionUI->SetActive(true);
		m_wpUIManager.lock()->SetTopPanel(m_spPalInteractionUI);

		m_wpSoundManager.lock()->PlaySoundEx("KeyInputSound");
	}
	else if (true == m_spPalInteractionUI->IsActive())
	{
		m_spPalInteractionUI->SetActive(false);
		m_wpUIManager.lock()->ResetTopPanel();

		m_wpSoundManager.lock()->PlaySoundEx("KeyInputSound");
	}
}

int32 Client::CPalInteractionSystem::InteractUIClick()
{
	if (-1 != m_iCurIndex)
	{
		m_wpSoundManager.lock()->PlaySoundEx("NormalClickSound");
		m_spPalInteractionUI->SetActive(false);
		m_wpUIManager.lock()->ResetTopPanel();
	}

	return m_iCurIndex;
}

void Client::CPalInteractionSystem::AddvecIndex()
{
	for (int32 i = 0; i < 3; i++)
	{
		FPalCircleUIInfo tPalCircleUIInfo;

		if (0 == i)
		{
			tPalCircleUIInfo.eMonsterInteractType = EMonsterInteractType::PET;
			tPalCircleUIInfo.fMinAngle = 0.0f;
			tPalCircleUIInfo.fMaxAngle = 120.0f;
		}
		else if (1 == i)
		{
			tPalCircleUIInfo.eMonsterInteractType = EMonsterInteractType::SLAUGHTER;
			tPalCircleUIInfo.fMinAngle = 120.f;
			tPalCircleUIInfo.fMaxAngle = 240.f;
		}
		else if (2 == i)
		{
			tPalCircleUIInfo.eMonsterInteractType = EMonsterInteractType::EAT;
			tPalCircleUIInfo.fMinAngle = 240.f;
			tPalCircleUIInfo.fMaxAngle = 360.f;
		}

		m_vecIndex.push_back({ i, tPalCircleUIInfo });
	}
}

void Client::CPalInteractionSystem::AddPlayerIndex()
{
	for (int32 i = 0; i < 3; i++)
	{
		FPlayerCircleUIInfo tPlayerCircleUIInfo;

		if (0 == i)
		{
			tPlayerCircleUIInfo.eEmoteType = EEmoteType::WAVE;
			tPlayerCircleUIInfo.fMinAngle = 0.0f;
			tPlayerCircleUIInfo.fMaxAngle = 120.0f;
		}
		else if (1 == i)
		{
			tPlayerCircleUIInfo.eEmoteType = EEmoteType::DANCE;
			tPlayerCircleUIInfo.fMinAngle = 120.f;
			tPlayerCircleUIInfo.fMaxAngle = 240.f;
		}
		else if (2 == i)
		{
			tPlayerCircleUIInfo.eEmoteType = EEmoteType::CHEER;
			tPlayerCircleUIInfo.fMinAngle = 240.f;
			tPlayerCircleUIInfo.fMaxAngle = 360.f;
		}

		m_vecPlayerIndex.push_back({ i, tPlayerCircleUIInfo });
	}
}

bool Client::CPalInteractionSystem::IsInAngle(float _fMinAngle, float _fMaxAngle, float _fDegreeAngle)
{
	if (_fMinAngle <= _fDegreeAngle && _fMaxAngle > _fDegreeAngle)
	{
		return true;
	}

	return false;
}