#include "pch.h"
#include "CombatSystem.h"

#include "Player.h"
#include "Monster.h"
#include "GameManager.h"

std::shared_ptr<CCombatSystem> Client::CCombatSystem::Create(const std::shared_ptr<FCombatSystemDesc>& _spCombatSystemDesc)
{
	shared_ptr<CCombatSystem> spCombatSystem = make_shared<CCombatSystem>();
	ENSUREF(spCombatSystem->Initialize(_spCombatSystemDesc), L"Failed to initialize combat system.");
	return spCombatSystem;
}

HRESULT Client::CCombatSystem::Initialize(const std::shared_ptr<FCombatSystemDesc>& _spCombatSystemDesc)
{
	m_wpGameManager = _spCombatSystemDesc->m_spGameManager;
	return S_OK;
}

HRESULT Client::CCombatSystem::BeginPlay()
{
	m_spPlayer = m_wpGameManager.lock()->GetPlayer();
	return S_OK;
}

int32 Client::CCombatSystem::PreTick(float _fDeltaSeconds)
{
	return 0;
}

int32 Client::CCombatSystem::Tick(float _fDeltaSeconds)
{
	return 0;
}

int32 Client::CCombatSystem::PostTick(float _fDeltaSeconds)
{
	return 0;
}

HRESULT Client::CCombatSystem::EndPlay()
{
	return S_OK;
}

void Client::CCombatSystem::Release()
{
	for (auto& pair : m_umapMonsters) { pair.second.reset(); }
	m_umapMonsters.clear();

	m_spPlayer = nullptr;
}

HRESULT Client::CCombatSystem::AddCombatPal(const std::shared_ptr<CMonster>& _spMonster)
{
	if (!_spMonster) { return E_FAIL; }

	CommandToWildPal(_spMonster);

	int32 iGameObjectID = _spMonster->GetGameObjectID();
	if (m_umapMonsters.contains(iGameObjectID)) { return E_FAIL; }

	m_umapMonsters.emplace(iGameObjectID, _spMonster);
	
	// 파트너 팰이 있는 경우
	CommandToPartnerPal();
	return S_OK;
}

HRESULT Client::CCombatSystem::RemoveCombatPal(const std::shared_ptr<CMonster>& _spMonster)
{
	if (!_spMonster) { return E_FAIL; }

	int32 iGameObjectID = _spMonster->GetGameObjectID();
	if (!m_umapMonsters.contains(iGameObjectID)) { return E_FAIL; }

	m_umapMonsters[iGameObjectID].reset();
	m_umapMonsters.erase(iGameObjectID);
	return S_OK;
}

HRESULT Client::CCombatSystem::RemoveCombatPal(int32 _iGameObjectID)
{
	if (_iGameObjectID < 0) { return E_FAIL; }

	if (!m_umapMonsters.contains(_iGameObjectID)) { return E_FAIL; }

	m_umapMonsters[_iGameObjectID].reset();
	m_umapMonsters.erase(_iGameObjectID);
	return S_OK;
}

HRESULT Client::CCombatSystem::CommandToPartnerPal()
{
	shared_ptr<CMonster> spPartnerPal = m_spPlayer->GetPartnerPal();

	// 파트너 팰이 없는 경우
	if (spPartnerPal == nullptr) { return S_OK; }

	// 전투 중인 야생 팰이 없는 경우
	if (m_umapMonsters.empty())
	{
		spPartnerPal->SetParticipateInCombat(false);
		spPartnerPal->ModifyBoolValue(EBlackboardBoolValue::BOOL_AGGRO, false);
		spPartnerPal->ModifyGameObjectValue(EBlackboardGameObjectValue::ATTACK_TARGET, nullptr);
		return S_OK;
	}

	for (const auto& pair : m_umapMonsters)
	{
		if (pair.second.expired()) { continue; }

		if (!pair.second.lock()->IsDead())
		{
			spPartnerPal->SetParticipateInCombat(true);
			spPartnerPal->ModifyBoolValue(EBlackboardBoolValue::BOOL_AGGRO, true);
			spPartnerPal->ModifyGameObjectValue(EBlackboardGameObjectValue::ATTACK_TARGET, pair.second.lock());
			break;
		}
	}
	return S_OK;
}

HRESULT Client::CCombatSystem::CommandToWildPal(const std::shared_ptr<CMonster>& _spMonster)
{
	if (_spMonster->GetMonsterAIType() != EMonsterAIType::WILD_PAL) { return S_OK; }
	
	shared_ptr<CGameObject> spGameObject = _spMonster->GetGameObjectValue(EBlackboardGameObjectValue::ATTACK_TARGET);
	if (!spGameObject)
	{
		_spMonster->ModifyGameObjectValue(EBlackboardGameObjectValue::ATTACK_TARGET, m_spPlayer);
	}
	
	return S_OK;
}
