#include "pch.h"
#include "Mineral.h"

// client managers
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

std::shared_ptr<CMineral> Client::CMineral::Create(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	shared_ptr<CMineral> spMineral = make_shared<CMineral>();
	ENSUREF(spMineral->Initialize(_spGameObjectDesc), L"Failed to initialize mineral.");
	return spMineral;
}

HRESULT Client::CMineral::Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	// item desc
	if (FAILED(CItem::Initialize(_spGameObjectDesc))) { return E_FAIL; }
	m_eItemKindType = EItemKindType::MINERAL;

	// mineral desc
	shared_ptr<FMineralDesc> spMineralDesc = static_pointer_cast<FMineralDesc>(_spGameObjectDesc);
	m_eMineralType = spMineralDesc->m_eCurMineralType;
	m_bPrefab = spMineralDesc->m_bPrefab;

	if (!m_bPrefab)
	{
		InitMeshComponent();
		InitColliderComponent();
	}

	return S_OK;
}

HRESULT Client::CMineral::BeginPlay()
{
	if (FAILED(CItem::BeginPlay())) { return E_FAIL; }

	return S_OK;
}

int32 Client::CMineral::PreTick(float _fDeltaSeconds)
{
	switch(CItem::PreTick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return 0;
}

int32 Client::CMineral::Tick(float _fDeltaSeconds)
{
	switch(CItem::Tick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return 0;
}

int32 Client::CMineral::PostTick(float _fDeltaSeconds)
{
	switch(CItem::PostTick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return 0;
}

HRESULT Client::CMineral::EndPlay()
{
	if (FAILED(CItem::EndPlay())) { return E_FAIL; }

	return S_OK;
}

void Client::CMineral::Release()
{
	CItem::Release();
}

bool Client::CMineral::IsCommandAssigned() const
{
	EToDoCommandState eCommandState = m_wpToDoSystem.lock()->GetCommandState(EToDoType::TRANSPORT_MINERAL, m_iCurCommandID);
	
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

void Client::CMineral::InitMeshComponent()
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

	switch (m_eMineralType)
	{
	case Client::EMineralType::IRON:		tMeshComponentDesc.m_wstrModelFilePath = L"../../Resource/Models/Dat/NonAnim/Props/SteelOre.dat";	break;// ö����
	case Client::EMineralType::STONE:		tMeshComponentDesc.m_wstrModelFilePath = L"../../Resource/Models/Dat/NonAnim/Props/Stone001.dat";	break;// �� ����
	case Client::EMineralType::WOOD:		tMeshComponentDesc.m_wstrModelFilePath = L"../../Resource/Models/Dat/NonAnim/Props/Wood001.dat";	break;// ���� ����
	case Client::EMineralType::WOOL:		tMeshComponentDesc.m_wstrModelFilePath = L"../../Resource/Models/Dat/NonAnim/Props/LeatherWool.dat";break;// ����
	case Client::EMineralType::FIBER:		tMeshComponentDesc.m_wstrModelFilePath = L"../../Resource/Models/Dat/NonAnim/Props/Fiber.dat";		break;
	default:								CHECKF(false, L"Mineral type is not designated.");													break;
	}
	
	m_spMeshComponent = CMeshComponent::Create(make_shared<FMeshComponentDesc>(tMeshComponentDesc));
	ENSURE(AddComponent(L"MeshComponent", m_spMeshComponent));
}

void Client::CMineral::InitColliderComponent()
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

int32 Client::CMineral::RegisterCommandToSystem(EToDoType _eToDoType, EMonsterTaskType _eMonsterTaskType)
{
	FToDoCommand tToDoCommand;
	tToDoCommand.m_eToDoType = _eToDoType;
	tToDoCommand.m_eMonsterTaskType = _eMonsterTaskType;
	tToDoCommand.m_spItem = dynamic_pointer_cast<CItem>(shared_from_this());

	return m_wpToDoSystem.lock()->AddCommandToSystem(tToDoCommand);
}

HRESULT Client::CMineral::UnRegisterCommandFromSystem(EToDoType _eToDoType, int32 _iCurCommandID)
{
	return m_wpToDoSystem.lock()->RemoveCommandFromSystem(_eToDoType, _iCurCommandID);
}

HRESULT Client::CMineral::AddWorkingSlavePal(const std::shared_ptr<CMonster>& _spMonster)
{
	if (!_spMonster) { return E_FAIL; }

	m_wpWorkingSlavePal = _spMonster;
	return S_OK;
}

HRESULT Client::CMineral::RemoveWorkingSlavePal()
{
	m_wpWorkingSlavePal.reset();
	return S_OK;
}

void Client::CMineral::SetPosition(const Vector3& _vPosition)
{
	if (!m_spBodySphereComponent) { return; }

	m_spBodySphereComponent->SetPosition(_vPosition);
}

const std::optional<Vector3> Client::CMineral::GetPosition() const
{
	if (!m_spBodySphereComponent) { return nullopt; }

	return m_spBodySphereComponent->GetPosition();
}

void Client::CMineral::SetGravity(bool _bGravity)
{
	m_spBodySphereComponent->SetUseGravity(_bGravity);
}

void Client::CMineral::OnTriggerEnter(CComponent* _pSrcComponent, CComponent* _pDstComponent)
{
	// �÷��̾� ��Ʈ�ѷ�
	if (_pDstComponent->GetComponentType() == EComponentType::CONTROLLER)
	{
		return;
	}

	// �ݶ��̴� ������Ʈ
	CColliderComponent* pColliderComponent = static_cast<CColliderComponent*>(_pDstComponent);
	
	// �������� �۾� �ý��ۿ� ����մϴ�.
	if (pColliderComponent->GetCollisionLayer() == ECollisionLayer::BASE_CAMP)
	{
		m_iCurCommandID = RegisterCommandToSystem(EToDoType::TRANSPORT_MINERAL, EMonsterTaskType::TRANSPORT);
	}
}

bool Client::CMineral::CollectItem()
{
	// �۾� �ý��ۿ� ��ϵ��� ���� �������� ���, ������ �� �����ϴ�.
	if (!IsCommandRegistered()) { return false; }

	// �̹� ���� �ӿ��� ����� �Ҵ�� ���, ������ �� �����ϴ�.
	if (IsCommandAssigned()) { return false; }

	// �۾� �ý��ۿ��� ����� öȸ�մϴ�.
	ENSURE(UnRegisterCommandFromSystem(EToDoType::TRANSPORT_MINERAL, m_iCurCommandID));
	
	// �κ��丮�� �������� �ֽ��ϴ�.
	m_wpInventorySystem.lock()->AddItemToContainer(static_pointer_cast<CItem>(shared_from_this()));

	// �������� �޸𸮿��� �����մϴ�.
	
	// SetActive(false);

	return true;
}

void Client::CMineral::SetPhysxTransformMatrix(const Matrix& _matTransform)
{
	CHECKF(m_spBodySphereComponent, L"Mineral collider component doesn't exist.");
	m_spBodySphereComponent->SetPhysxTransformMatrix(_matTransform);
}
