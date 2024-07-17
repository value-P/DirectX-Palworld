#include "pch.h"
#include "DecoratorNear.h"

#include "Player.h"
#include "Monster.h"

std::shared_ptr<CDecoratorNear> Client::CDecoratorNear::Create(const std::shared_ptr<FDecoratorNearDesc>& _spDecoratorNearDesc)
{
	const std::shared_ptr<CDecoratorNear> spDecoratorNear = make_shared<CDecoratorNear>();
	ENSUREF(spDecoratorNear->Initialize(_spDecoratorNearDesc), L"Failed to initialize decorator near node.");
	return spDecoratorNear;
}

HRESULT Client::CDecoratorNear::Initialize(const std::shared_ptr<FBTNodeDesc>& _spNodeDesc)
{
	// deco desc
	if (FAILED(CDecoratorNode::Initialize(_spNodeDesc))) { return E_FAIL; }

	// dead deco desc
	shared_ptr<FDecoratorNearDesc> spDecoNearDesc = static_pointer_cast<FDecoratorNearDesc>(_spNodeDesc);
	m_spMonster = dynamic_pointer_cast<CMonster>(m_wpOwnerGameObject.lock());
	m_spPlayer	= spDecoNearDesc->m_spPlayer;
	m_fRadius	= spDecoNearDesc->m_fRadius;
	return S_OK;
}

bool Client::CDecoratorNear::EvaluateDecoNode()
{ 
	Vector3 vPlayerPosition = m_spPlayer->GetPosition().value();
	Vector3 vMonsterPosition = m_spMonster->GetPosition().value();

	float fDist = Vector3::Distance(vPlayerPosition, vMonsterPosition);

	if (fDist <= m_fRadius && !m_spMonster->IsCaptured())
	{
		return true;
	}

	return false;
}

void Client::CDecoratorNear::Release()
{
	m_spMonster = nullptr;
	m_spPlayer = nullptr;

	CDecoratorNode::Release();
}
