#include "pch.h"
#include "DecoratorPartnerSkill.h"

// client
#include "ClientLoop.h"

// gameobject
#include "Monster.h"

// component
#include "BehaviorTreeComponent.h"
#include "SphereComponent.h"
#include "BoxComponent.h"

std::shared_ptr<CDecoratorPartnerSkill> Client::CDecoratorPartnerSkill::Create(const std::shared_ptr<FDecoratorPartnerSkillDesc>& _spDecoratorPartnerSkillDesc)
{
	const std::shared_ptr<CDecoratorPartnerSkill> spDecoratorPartnerSkill = make_shared<CDecoratorPartnerSkill>();
	ENSUREF(spDecoratorPartnerSkill->Initialize(_spDecoratorPartnerSkillDesc), L"Failed to initialize partner skill decorator node.");
	return spDecoratorPartnerSkill;
}

HRESULT Client::CDecoratorPartnerSkill::Initialize(const std::shared_ptr<FBTNodeDesc>& _spNodeDesc)
{
	// deco desc
	if (FAILED(CDecoratorNode::Initialize(_spNodeDesc))) { return E_FAIL; }

	// interact deco desc
	shared_ptr<FDecoratorPartnerSkillDesc> spDecoPartnerSkillDesc = static_pointer_cast<FDecoratorPartnerSkillDesc>(_spNodeDesc);

	// etc
	m_spMonster = dynamic_pointer_cast<CMonster>(m_wpOwnerGameObject.lock());
	return S_OK;
}

bool Client::CDecoratorPartnerSkill::EvaluateDecoNode()
{
	// 파트너 스킬 사용함으로 변경한 경우
	if (m_spMonster->GetBoolValue(EBlackboardBoolValue::BOOL_PARTNER_SKILL) == true)
	{
		if (m_bCurPartnerSkill == false)
		{
			shared_ptr<CColliderComponent> spColliderComponent = m_spMonster->GetBodyColliderComponent();

			float fAcceptableRadius = 0.0f;

			switch (spColliderComponent->GetComponentType())
			{
			case EComponentType::SPHERE:
			{
				shared_ptr<CSphereComponent> spSphereComponent = static_pointer_cast<CSphereComponent>(spColliderComponent);
				fAcceptableRadius = spSphereComponent->GetRadius();
			}
			break;

			case EComponentType::BOX:
			{
				shared_ptr<CBoxComponent> spBoxComponent = static_pointer_cast<CBoxComponent>(spColliderComponent);
				Vector3 vExtents = spBoxComponent->GetExtents();

				fAcceptableRadius = max(vExtents.x, vExtents.z);
			}
			break;

			default:
				CHECKF(false, L"Collider type is not designated.");
				break;
			}
			
			fAcceptableRadius += CClientLoop::s_spClientDataSheet->k_fPlayerInteractRadius;
			
			m_spMonster->ModifyFloatValue(EBlackboardFloatValue::FLOAT_ACCEPTABLE_RADIUS, fAcceptableRadius);
			m_bCurPartnerSkill = true;
		}

		return true;
	}

	// 파트너 스킬 사용하지 않음으로 변경한 경우
	else
	{
		if (m_bCurPartnerSkill == true)
		{
			m_wpBehaviorTreeComponent.lock()->ResetBTNodeResult(m_spParentBTNode);
			m_wpBehaviorTreeComponent.lock()->ResetBehindBTNodeResult();
			m_bCurPartnerSkill = false;
		}
	}

	return false;
}

void Client::CDecoratorPartnerSkill::Release()
{
	m_spMonster = nullptr;

	CDecoratorNode::Release();
}
