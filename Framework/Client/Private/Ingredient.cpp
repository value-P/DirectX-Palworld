#include "pch.h"
#include "Ingredient.h"

// client manager
#include "GameManager.h"
#include "ToDoSystem.h"

// component
#include "Component.h"
#include "ColliderComponent.h"
#include "SphereComponent.h"
#include "MeshComponent.h"

// gameobject
#include "Monster.h"

// system
#include "InventorySystem.h"

std::shared_ptr<CIngredient> Client::CIngredient::Create(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	shared_ptr<CIngredient> spInstance = make_shared<CIngredient>();
	spInstance->Initialize(_spGameObjectDesc);
	return spInstance;
}

HRESULT Client::CIngredient::Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	// item desc
	if (FAILED(CItem::Initialize(_spGameObjectDesc))) { return E_FAIL; }
	m_eItemKindType = EItemKindType::INGREDIENT;

	// ingredient desc
	shared_ptr<FIngredientDesc> spIngredientDesc = static_pointer_cast<FIngredientDesc>(_spGameObjectDesc);
	m_eIngredientType = spIngredientDesc->m_eIngredientType;
	m_bPrefab = spIngredientDesc->m_bPrefab;

	// etc
	m_wpToDoSystem = CGameManager::GetInstance()->GetToDoSystem();

	if (!m_bPrefab)
	{
		InitMeshComponent();
		InitColliderComponent();
	}

	return S_OK;
}

HRESULT Client::CIngredient::BeginPlay()
{
	if (FAILED(CItem::BeginPlay())) { return E_FAIL; }

	return S_OK;
}

int32 Client::CIngredient::PreTick(float _fDeltaSeconds)
{
	switch(CItem::PreTick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return 0;
}

int32 Client::CIngredient::Tick(float _fDeltaSeconds)
{
	switch(CItem::Tick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return 0;
}

int32 Client::CIngredient::PostTick(float _fDeltaSeconds)
{
	switch(CItem::PostTick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return 0;
}

HRESULT Client::CIngredient::EndPlay()
{
	if (FAILED(CItem::EndPlay())) { return E_FAIL; }

	return S_OK;
}

void Client::CIngredient::Release()
{
	m_spBodySphereComponent = nullptr;

	CItem::Release();
}

bool Client::CIngredient::IsCommandAssigned() const
{
	EToDoCommandState eCommandState = m_wpToDoSystem.lock()->GetCommandState(EToDoType::TRANSPORT_INGREDIENT, m_iCurCommandID);

	switch (eCommandState)
	{
	case Client::EToDoCommandState::NOT_ASSIGNED:
		return false;

	case Client::EToDoCommandState::STARTED:
	case Client::EToDoCommandState::PAUSED:
	case Client::EToDoCommandState::FINISHED:
		return true;

	default:
		CHECKF(false, L"To do command state is not designated.");
		break;
	}

	return false;
}

void Client::CIngredient::InitMeshComponent()
{
	// component desc
	FMeshComponentDesc tMeshComponentDesc;
	tMeshComponentDesc.m_eRenderType		= ERenderType::RENDER_NBLEND;
	tMeshComponentDesc.m_eComponentType		= EComponentType::MESH;
	tMeshComponentDesc.m_wpOwnerGameObject	= shared_from_this();
	tMeshComponentDesc.m_wstrComponentName	= L"MeshComponent";

	// mesh component
	tMeshComponentDesc.m_matPivot			= Matrix::CreateScale(0.0001f) * Matrix::CreateRotationY(XMConvertToRadians(180.f));
	tMeshComponentDesc.m_eMeshType			= EMeshManageType::UNIQUE;

	switch (m_eIngredientType)
	{
	case Client::EIngredientType::RED_BERRY:		tMeshComponentDesc.m_wstrModelFilePath = L"../../Resource/Models/Dat/NonAnim/Props/BerryRed.dat";	break;// 열매
	case Client::EIngredientType::EGG:				tMeshComponentDesc.m_wstrModelFilePath = L"../../Resource/Models/Dat/NonAnim/Props/Egg.dat";		break;// 알
	case Client::EIngredientType::SHEEPBALL_MEAT:	tMeshComponentDesc.m_wstrModelFilePath = L"../../Resource/Models/Dat/NonAnim/Props/MeatRaw.dat";	break;// 도로롱 고기
	case Client::EIngredientType::BOAR_MEAT:		tMeshComponentDesc.m_wstrModelFilePath = L"../../Resource/Models/Dat/NonAnim/Props/MeatRaw.dat";	break;// 돌진 돼지 고기
	default:										CHECKF(false, L"Mineral type is not designated.");													break;
	}

	m_spMeshComponent = CMeshComponent::Create(make_shared<FMeshComponentDesc>(tMeshComponentDesc));
	ENSURE(AddComponent(L"MeshComponent", m_spMeshComponent));
}

void Client::CIngredient::InitColliderComponent()
{
	// component desc
	FSphereComponentDesc tSphereComponentDesc;
	tSphereComponentDesc.m_wpOwnerGameObject	= shared_from_this();
	tSphereComponentDesc.m_wstrComponentName	= L"SphereComponent";
	tSphereComponentDesc.m_eComponentType		= EComponentType::SPHERE;
	tSphereComponentDesc.m_eRenderType			= ERenderType::RENDER_DEBUG;

	// collider component desc
	tSphereComponentDesc.m_bGravity				= true;
	tSphereComponentDesc.m_bTrigger				= false;
	tSphereComponentDesc.m_iAxisLockType		= (int32)EAxisLockType::LOCK_ANGULAR_X | (int32)EAxisLockType::LOCK_ANGULAR_Y | (int32)EAxisLockType::LOCK_ANGULAR_Z;
	tSphereComponentDesc.m_eColliderType		= EColliderType::SPHERE;
	tSphereComponentDesc.m_eCollisionFlag		= ECollisionFlag::ITEM;
	tSphereComponentDesc.m_eCollisionLayer		= ECollisionLayer::ITEM;
	tSphereComponentDesc.m_eComponentType		= EComponentType::SPHERE;
	tSphereComponentDesc.m_eMobilityType		= EMobilityType::DYNAMIC;
	tSphereComponentDesc.m_vLocalPosition		= Vector3(0.0f, 0.2f, 0.0f);

	// sphere component desc
	tSphereComponentDesc.m_fRadius				= 0.2f;

	m_spBodySphereComponent = CSphereComponent::Create(make_shared<FSphereComponentDesc>(tSphereComponentDesc));
	ENSURE(AddComponent(tSphereComponentDesc.m_wstrComponentName, m_spBodySphereComponent));
}

int32 Client::CIngredient::RegisterCommandToSystem(EToDoType _eToDoType, EMonsterTaskType _eMonsterTaskType)
{
	FToDoCommand tToDoCommand;
	tToDoCommand.m_eToDoType = _eToDoType;
	tToDoCommand.m_eMonsterTaskType = _eMonsterTaskType;
	tToDoCommand.m_spItem = dynamic_pointer_cast<CItem>(shared_from_this());

	return m_wpToDoSystem.lock()->AddCommandToSystem(tToDoCommand);
}

HRESULT Client::CIngredient::UnRegisterCommandFromSystem(EToDoType _eToDoType, int32 _iCurCommandID)
{
	return m_wpToDoSystem.lock()->RemoveCommandFromSystem(_eToDoType, _iCurCommandID);
}

HRESULT Client::CIngredient::AddWorkingSlavePal(const std::shared_ptr<CMonster>& _spMonster)
{
	if (!_spMonster) { return E_FAIL; }

	m_wpWorkingSlavePal = _spMonster;
	return S_OK;
}

HRESULT Client::CIngredient::RemoveWorkingSlavePal()
{
	m_wpWorkingSlavePal.reset();
	return S_OK;
}

void Client::CIngredient::SetPosition(const Vector3& _vPosition)
{
	if (!m_spBodySphereComponent) { return; }

	m_spBodySphereComponent->SetPosition(_vPosition);
}

const std::optional<Vector3> Client::CIngredient::GetPosition() const
{
	if (!m_spBodySphereComponent) { return nullopt; }

	return m_spBodySphereComponent->GetPosition();
}

void Client::CIngredient::SetGravity(bool _bGravity)
{
	m_spBodySphereComponent->SetUseGravity(_bGravity);
}

void Client::CIngredient::OnTriggerEnter(CComponent* _pSrcComponent, CComponent* _pDstComponent)
{
	// 플레이어 컨트롤러
	if (_pDstComponent->GetComponentType() == EComponentType::CONTROLLER)
	{
		return;
	}

	// 콜라이더 컴포넌트
	CColliderComponent* pColliderComponent = static_cast<CColliderComponent*>(_pDstComponent);
	
	// 아이템을 작업 시스템에 등록합니다.
	if (pColliderComponent->GetCollisionLayer() == ECollisionLayer::BASE_CAMP)
	{
		m_iCurCommandID = RegisterCommandToSystem(EToDoType::TRANSPORT_INGREDIENT, EMonsterTaskType::TRANSPORT);
	}
}

bool Client::CIngredient::CollectItem()
{
	// 작업 시스템에 등록되지 않은 아이템인 경우, 수집할 수 없습니다.
	if (!IsCommandRegistered()) { return false; }

	// 이미 거점 팰에게 명령이 할당된 경우, 수집할 수 없습니다.
	if (IsCommandAssigned()) { return false; }

	// 작업 시스템에서 명령을 철회합니다.
	ENSURE(m_wpToDoSystem.lock()->RemoveCommandFromSystem(EToDoType::TRANSPORT_INGREDIENT, m_iCurCommandID));

	// 인벤토리에 아이템을 넣습니다.
	m_wpInventorySystem.lock()->AddItemToContainer(static_pointer_cast<CItem>(shared_from_this()));

	// 아이템을 메모리에서 해제합니다.

	return true;
}

void Client::CIngredient::SetPhysxTransformMatrix(const Matrix& _matTransform)
{
	CHECKF(m_spBodySphereComponent, L"Ingredient collider component doesn't exist.");
	m_spBodySphereComponent->SetPhysxTransformMatrix(_matTransform);
}
