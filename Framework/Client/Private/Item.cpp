#include "pch.h"
#include "Item.h"

// client manager
#include "GameManager.h"

// component
#include "SphereComponent.h"

// system
#include "ToDoSystem.h"
#include "InventorySystem.h"

// gameobject
#include "Monster.h"
#include "Architecture.h"

// engine
#include "Bone.h"

HRESULT Client::CItem::Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	// gameobject desc
	if (FAILED(CGameObject::Initialize(_spGameObjectDesc))) { return E_FAIL; }

	// item desc
	shared_ptr<FItemDesc> spItemDesc = static_pointer_cast<FItemDesc>(_spGameObjectDesc);
	m_eCurItemType	= spItemDesc->m_eCurItemType;
	m_eItemKindType = spItemDesc->m_eCurItemKindType;
	m_iItemCount	= spItemDesc->m_iItemCount;

	// etc
	m_wpToDoSystem = CGameManager::GetInstance()->GetToDoSystem();
	m_wpInventorySystem = CGameManager::GetInstance()->GetInventorySystem();
	return S_OK;
}

HRESULT Client::CItem::BeginPlay()
{
	if (FAILED(CGameObject::BeginPlay())) { return E_FAIL; }

	return S_OK;
}

int32 Client::CItem::PreTick(float _fDeltaSeconds)
{
	switch(CGameObject::PreTick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}
	return 0;
}

int32 Client::CItem::Tick(float _fDeltaSeconds)
{
	switch(CGameObject::Tick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}
	return 0;
}

int32 Client::CItem::PostTick(float _fDeltaSeconds)
{
	switch(CGameObject::PostTick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}
	return 0;
}

HRESULT Client::CItem::EndPlay()
{
	if (FAILED(CGameObject::EndPlay())) { return E_FAIL; }

	return S_OK;
}

void Client::CItem::Release()
{
	for (auto& pair : m_umapWorkingSlavePals) { pair.second.reset(); }
	m_umapWorkingSlavePals.clear();

	m_spMeshComponent = nullptr;

	CGameObject::Release();
}

void Client::CItem::TransportItem(const std::shared_ptr<CBone>& _spBone, const Matrix& _matWorld)
{
	Matrix matTransform = _spBone->GetCombinedTransformationMatrix();
	matTransform *= _matWorld;

	Vector3 vPosition = matTransform.Translation();
	SetPosition(vPosition);
	LookAt(vPosition + GetForwardVector().value());
}

void Client::CItem::SetPhysxTransformMatrix(const Matrix& _matTransform)
{
}

bool Client::CItem::CollectItem()
{
	return false;
}

int32 Client::CItem::RegisterCommandToSystem(EToDoType _eToDoType, EMonsterTaskType _eMonsterTaskType)
{
	FToDoCommand tToDoCommand;
	tToDoCommand.m_eToDoType = _eToDoType;
	tToDoCommand.m_eMonsterTaskType = _eMonsterTaskType;
	tToDoCommand.m_spItem = dynamic_pointer_cast<CItem>(shared_from_this());
	tToDoCommand.m_spHabitat = m_wpToDoSystem.lock()->FindArchitecture(EArchitectureType::WOODEN_CHEST);

	return m_wpToDoSystem.lock()->AddCommandToSystem(tToDoCommand);
}

HRESULT Client::CItem::UnRegisterCommandFromSystem(EToDoType _eToDoType, int32 _iCurCommandID)
{
	if (_iCurCommandID == -1) { _iCurCommandID = m_iCurCommandID; }

	return m_wpToDoSystem.lock()->RemoveCommandFromSystem(_eToDoType, _iCurCommandID);
}

HRESULT Client::CItem::AddWorkingSlavePal(const std::shared_ptr<CMonster>& _spMonster)
{
	int32 iGameObjectID = _spMonster->GetGameObjectID();
	if (m_umapWorkingSlavePals.contains(iGameObjectID)) { return E_FAIL; }

	m_umapWorkingSlavePals.emplace(iGameObjectID, _spMonster);
	return S_OK;
}

HRESULT Client::CItem::RemoveWorkingSlavePal(int32 _iGameObjectID)
{
	if (!m_umapWorkingSlavePals.contains(_iGameObjectID)) { return E_FAIL; }

	m_umapWorkingSlavePals[_iGameObjectID].reset();
	m_umapWorkingSlavePals.erase(_iGameObjectID);
	return S_OK;
}
