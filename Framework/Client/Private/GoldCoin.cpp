#include "pch.h"
#include "GoldCoin.h"

// manager
#include "GameManager.h"

// system
#include "InventorySystem.h"

// component
#include "MeshComponent.h"
#include "SphereComponent.h"

std::shared_ptr<CGoldCoin> Client::CGoldCoin::Create(const std::shared_ptr<FGoldCoinDesc>& _spGoldCoinDesc)
{
	std::shared_ptr<CGoldCoin> spGoldCoin = make_shared<CGoldCoin>();
	ENSUREF(spGoldCoin->Initialize(_spGoldCoinDesc), L"Failed to initialize gold coin");
	return spGoldCoin;
}

std::shared_ptr<CEffectGroup> Client::CGoldCoin::CreateFromJson(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc, const std::wstring& _wstrJsonFileName)
{
	std::shared_ptr<CGoldCoin> spGoldCoin = make_shared<CGoldCoin>();
	ENSUREF(spGoldCoin->InitializeFromJson(_spGameObjectDesc, _wstrJsonFileName), L"Failed to initialize gold coin");
	return spGoldCoin;
}

HRESULT Client::CGoldCoin::Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	if (FAILED(CEffectGroup::Initialize(_spGameObjectDesc))) { return E_FAIL; }

	m_wpInventorySystem = CGameManager::GetInstance()->GetInventorySystem();

	InitMeshComponent();
	InitColliderComponent();

	return S_OK;
}

HRESULT Client::CGoldCoin::InitializeFromJson(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc, const std::wstring& _wstrJsonFileName)
{
	if (FAILED(CEffectGroup::InitializeFromJson(_spGameObjectDesc, _wstrJsonFileName))) { return E_FAIL; }

	m_wpInventorySystem = CGameManager::GetInstance()->GetInventorySystem();

	InitColliderComponent();

	return S_OK;
}

HRESULT Client::CGoldCoin::BeginPlay()
{
	if (FAILED(CEffectGroup::BeginPlay())) { return E_FAIL; }

	return S_OK;
}

int32 Client::CGoldCoin::PreTick(float _fDeltaSeconds)
{
	switch (CEffectGroup::PreTick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return S_OK;
}

int32 Client::CGoldCoin::Tick(float _fDeltaSeconds)
{
	switch (CEffectGroup::Tick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	Turn(Vector3(0,1,0), 5.0f);

	return S_OK;
}

int32 Client::CGoldCoin::PostTick(float _fDeltaSeconds)
{
	switch (CEffectGroup::PostTick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return S_OK;
}

HRESULT Client::CGoldCoin::EndPlay()
{
	if (FAILED(CEffectGroup::EndPlay())) { return E_FAIL; }

	return S_OK;
}

void Client::CGoldCoin::Release()
{

	CEffectGroup::Release();
}

void Client::CGoldCoin::OnTriggerEnter(CComponent* _pSrcComponent, CComponent* _pDstComponent)
{
	// 컨트롤러 컴포넌트
	if (_pDstComponent->GetComponentType() == EComponentType::CONTROLLER) 
	{
		// 인벤토리에 동전 집어 넣기
		m_wpInventorySystem.lock()->AddItemToContainer(EItemType::GOLD_COIN, 1);

		// 플레이어와 충돌했을때
		SetActive(false); 

		// 먹는 이펙트 플레이어 주위에 띄우기
	}
}

void Client::CGoldCoin::OnTimerOver(const shared_ptr<CTimer>& _spTimer)
{
	// 알람 같은 동작을 합니다.
	SetActive(false);
}

void Client::CGoldCoin::InitMeshComponent()
{
	// component
	FMeshComponentDesc tMeshComponentDesc;
	tMeshComponentDesc.m_eComponentType    = EComponentType::MESH;
	tMeshComponentDesc.m_eRenderType	   = ERenderType::RENDER_NBLEND;
	tMeshComponentDesc.m_wpOwnerGameObject = shared_from_this();
	tMeshComponentDesc.m_wstrComponentName = L"MeshComponent";

	// mesh component desc
	tMeshComponentDesc.m_wstrModelFilePath = L"../../Resource/Models/Dat/NonAnim/Props/Coin.dat";
	tMeshComponentDesc.m_matPivot		   = Matrix::CreateScale(0.002f) * Matrix::CreateRotationY(XMConvertToRadians(180.0f));
	tMeshComponentDesc.m_wstrShaderName    = L"";

	m_spMeshComponent = CMeshComponent::Create(make_shared<FMeshComponentDesc>(tMeshComponentDesc));
	ENSURE(AddComponent(tMeshComponentDesc.m_wstrComponentName, m_spMeshComponent));
}

void Client::CGoldCoin::InitColliderComponent()
{
	// component
	FSphereComponentDesc tSphereComponentDesc;
	tSphereComponentDesc.m_eComponentType    = EComponentType::SPHERE;
	tSphereComponentDesc.m_eRenderType		 = ERenderType::RENDER_DEBUG;
	tSphereComponentDesc.m_wpOwnerGameObject = shared_from_this();
	tSphereComponentDesc.m_wstrComponentName = L"GoldCoin Sphere Component";

	// collider component desc
	tSphereComponentDesc.m_eMobilityType   = EMobilityType::DYNAMIC;
	tSphereComponentDesc.m_eColliderType   = EColliderType::SPHERE;
	tSphereComponentDesc.m_eCollisionFlag  = ECollisionFlag::MINIGAME_GACHA;
	tSphereComponentDesc.m_eCollisionLayer = ECollisionLayer::MINIGAME_GACHA;
	tSphereComponentDesc.m_vLocalPosition  = Vector3(0.f, 0.f, 0.f);
	tSphereComponentDesc.m_bGravity		   = false;
	tSphereComponentDesc.m_bTrigger		   = true;
	tSphereComponentDesc.m_fDensity		   = 2.5f;

	tSphereComponentDesc.m_fRadius = 1.f;

	m_spSphereComponent = CSphereComponent::Create(make_shared<FSphereComponentDesc>(tSphereComponentDesc));
	AddComponent(tSphereComponentDesc.m_wstrComponentName, m_spSphereComponent);
}
