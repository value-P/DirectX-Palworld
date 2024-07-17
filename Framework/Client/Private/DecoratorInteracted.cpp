#include "pch.h"
#include "DecoratorInteracted.h"

// client
#include "ClientLoop.h"

// component
#include "SphereComponent.h"
#include "BoxComponent.h"

// gameobject
#include "Monster.h"

std::shared_ptr<CDecoratorInteracted> Client::CDecoratorInteracted::Create(const std::shared_ptr<FDecoratorInteractedDesc>& _spDecoratorInteractedDesc)
{
	const std::shared_ptr<CDecoratorInteracted> spDecoratorInteracted = make_shared<CDecoratorInteracted>();
	ENSUREF(spDecoratorInteracted->Initialize(_spDecoratorInteractedDesc), L"Failed to initialize interact decorator node.");
	return spDecoratorInteracted;
}

HRESULT Client::CDecoratorInteracted::Initialize(const std::shared_ptr<FBTNodeDesc>& _spNodeDesc)
{
	// deco desc
	if (FAILED(CDecoratorNode::Initialize(_spNodeDesc))) { return E_FAIL; }

	// interact deco desc
	shared_ptr<FDecoratorInteractedDesc> spDecoInteractedDesc = static_pointer_cast<FDecoratorInteractedDesc>(_spNodeDesc);

	// etc
	m_spMonster = dynamic_pointer_cast<CMonster>(m_wpOwnerGameObject.lock());
	return S_OK;
}

bool Client::CDecoratorInteracted::EvaluateDecoNode()
{
	if (m_spMonster->GetBoolValue(EBlackboardBoolValue::BOOL_INTERACTED))
	{
		if (m_bInit) { return true; }
		
		m_bInit = true;
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
		
		// 작업 중에 상호작용하는 경우, 작업을 일시정지하고 반환합니다.
		m_spMonster->PauseToDoCommand();
		return true;
	}

	m_bInit = false;
	return false;
}

void Client::CDecoratorInteracted::Release()
{
	m_spMonster = nullptr;

	CDecoratorNode::Release();
}
