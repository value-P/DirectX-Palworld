#include "pch.h"
#include "TaskNotice.h"

// manager
#include "EffectManager.h"

// component
#include "MeshComponent.h"

// gameobject
#include "Monster.h"

std::shared_ptr<CTaskNotice> Client::CTaskNotice::Create(const std::shared_ptr<FTaskNoticeDesc>& _spTaskNoticeDesc)
{
	const std::shared_ptr<CTaskNotice> spTaskNotice = make_shared<CTaskNotice>();
	ENSUREF(spTaskNotice->Initialize(_spTaskNoticeDesc), L"Failed to initialize notice task.");
	return spTaskNotice;
}

HRESULT Client::CTaskNotice::Initialize(const shared_ptr<FBTNodeDesc>& _spNodeDesc)
{
	// task desc
	if (FAILED(CTaskNode::Initialize(_spNodeDesc))) { return E_FAIL; }

	// notice task desc
	shared_ptr<FTaskNoticeDesc> spTaskNoticeDesc = static_pointer_cast<FTaskNoticeDesc>(_spNodeDesc);
	m_iAnimationID = spTaskNoticeDesc->m_iAnimationID;
	m_eMonsterNoticeType = spTaskNoticeDesc->m_eMonsterNoticeType;

	// etc
	m_wpEffectManager = CEffectManager::GetInstance();
	m_spMonster = dynamic_pointer_cast<CMonster>(m_wpOwnerGameObject.lock());
	m_spMeshComponent = m_spMonster->GetMeshComponent();
	return S_OK;
}

void Client::CTaskNotice::OnNodeInit(float _fDeltaSecond)
{
	m_vPrePosition = nullopt;
	m_fSumNoticeDeltaSeconds = 0.0f;
}

void Client::CTaskNotice::OnNodeEnter(float _fDeltaSecond)
{
	// 표정을 변경합니다.
	m_spMonster->ChangeEye(EPalEyeType::DEFAULT);
	m_spMonster->ChangeMouth(EPalMouthType::DEFAULT);

	m_spMeshComponent->ChangeAnimation(m_iAnimationID, false);
	
	Vector3 vScale = m_spMonster->GetScale().value();
	vScale.x = vScale.z = 0.0f;

	switch (m_spMonster->GetMonsterSizeType())
	{
	case EMonsterSizeType::LARGE:
		vScale.y *= 3.0f;
		break;

	case EMonsterSizeType::MIDUM:
		vScale.y *= 2.5f;
		break;

	case EMonsterSizeType::SMALL:
		m_spMonster->GetMonsterType() == EMonsterType::DREAM_DEMON ? vScale.y *= 2.0f : vScale.y *= 1.2f;
		m_spMonster->GetMonsterType() == EMonsterType::BOAR ? vScale.y *= 1.8f : vScale.y *= 1.2f;
		break;

	default:
		CHECKF(false, L"Monster personality type is not designated.");
		break;
	}

	switch (m_eMonsterNoticeType)
	{
	case Client::EMonsterNoticeType::EXCLAMATION:
	{
		m_wpEffectManager.lock()->ActivateEffectCombined(L"Group_Notice01", m_spMonster, vScale);
		break;
	}
	case Client::EMonsterNoticeType::QUESTION:
	{
		m_wpEffectManager.lock()->ActivateEffectCombined(L"Group_Notice00", m_spMonster, vScale);
		break;
	}
	case Client::EMonsterNoticeType::SURPRISED:
	{
		// 놀라는 표시는 팰 우측 상단에 위치시키기 어려워 느낌표로 대체했습니다.
		m_wpEffectManager.lock()->ActivateEffectCombined(L"Group_Notice01", m_spMonster, vScale);
		break;
	}
	case Client::EMonsterNoticeType::LIGHT_BULB:
	{
		m_wpEffectManager.lock()->ActivateEffectCombined(L"Group_Notice03", m_spMonster, vScale);
		break;
	}

	default:
		CHECKF(false, L"Monster notice type is not designated.");
		break;
	}
}

EBTNodeResult Client::CTaskNotice::ExecuteTaskNode(float _fDeltaSecond)
{
	if (!m_vPrePosition.has_value())
	{
		m_vPrePosition = m_spMonster->GetPosition().value();
	}
	m_spMonster->SetPosition(m_vPrePosition.value());

	m_fSumNoticeDeltaSeconds += _fDeltaSecond;
	if (m_fMaxNoticeDeltaSeconds <= m_fSumNoticeDeltaSeconds)
	{
		m_fSumNoticeDeltaSeconds = 0.0f;
		m_vPrePosition = nullopt;
		return EBTNodeResult::SUCCEEDED;
	}

	return EBTNodeResult::IN_PROGRESS;
}

void Client::CTaskNotice::Release()
{
	m_spMeshComponent = nullptr;
	m_spMonster = nullptr;

	CBTNode::Release();
}
