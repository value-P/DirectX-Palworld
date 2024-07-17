#include "pch.h"
#include "TreasureBox.h"

// component
#include "MeshComponent.h"
#include "SphereComponent.h"

std::shared_ptr<CTreasureBox> Client::CTreasureBox::Create(const std::shared_ptr<FTreasureBoxDesc>& _spTreasureBoxDesc)
{
    std::shared_ptr<CTreasureBox> spTreasureBox = make_shared<CTreasureBox>();
    ENSUREF(spTreasureBox->Initialize(_spTreasureBoxDesc), L"Failed to initialize treasure box");
    return spTreasureBox;
}

HRESULT Client::CTreasureBox::Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
    if (FAILED(CGameObject::Initialize(_spGameObjectDesc))) { return E_FAIL; }

    InitMeshComponent();
    InitColliderComponent();

    return S_OK;
}

HRESULT Client::CTreasureBox::BeginPlay()
{
    if (FAILED(CGameObject::BeginPlay())) { return E_FAIL; }

    return S_OK;
}

int32 Client::CTreasureBox::PreTick(float _fDeltaSeconds)
{
    switch (CGameObject::PreTick(_fDeltaSeconds))
    {
    case -1:	return -1;
    default:	break;
    }

    return S_OK;
}

int32 Client::CTreasureBox::Tick(float _fDeltaSeconds)
{
    switch (CGameObject::Tick(_fDeltaSeconds))
    {
    case -1:	return -1;
    default:	break;
    }

    return S_OK;
}

int32 Client::CTreasureBox::PostTick(float _fDeltaSeconds)
{
    switch (CGameObject::PostTick(_fDeltaSeconds))
    {
    case -1:	return -1;
    default:	break;
    }

    return S_OK;
}

HRESULT Client::CTreasureBox::EndPlay()
{
    if (FAILED(CGameObject::EndPlay())) { return E_FAIL; }

    return S_OK;
}

void Client::CTreasureBox::Release()
{

    CGameObject::Release();
}

void Client::CTreasureBox::OnTriggerEnter(CComponent* _pSrcComponent, CComponent* _pDstComponent)
{
    // ��Ʈ�ѷ� ������Ʈ
    if (_pDstComponent->GetComponentType() == EComponentType::CONTROLLER) 
    { 
        // ���� ���ڰ� ������ �ִϸ޴ϼ� ���
        //m_spMeshComponent->ChangeAnimation();

        // �ִϸ޴ϼ� ��� ������
        SetActive(false);
    }
}

void Client::CTreasureBox::ReturnRandomReward()
{
    // ���� ���� ���� ����: 1.�ð� �߰�; 2.�� �߰�; 3.�ð� ����; 4.�ӵ� �ν��� ȿ��
    // ��� ���� ���� �ִ�: 1.��Ʈ�� ���� �ٲ��; 2.�ð��� ���� �帣��; 3.���� �̻������� 
}

void Client::CTreasureBox::InitMeshComponent()
{
   // component
	FMeshComponentDesc tMeshComponentDesc;
	tMeshComponentDesc.m_eComponentType    = EComponentType::MESH;
	tMeshComponentDesc.m_eRenderType	   = ERenderType::RENDER_NBLEND;
	tMeshComponentDesc.m_wpOwnerGameObject = shared_from_this();
	tMeshComponentDesc.m_wstrComponentName = L"MeshComponent";

	// mesh component desc
	tMeshComponentDesc.m_wstrModelFilePath = L"../../Resource/Models/Dat/NonAnim/Props/Coin.dat";
	tMeshComponentDesc.m_matPivot		   = Matrix::CreateScale(0.002f) * Matrix::CreateRotationY(XMConvertToRadians(180.f));
	tMeshComponentDesc.m_wstrShaderName    = L"";

	m_spMeshComponent = CMeshComponent::Create(make_shared<FMeshComponentDesc>(tMeshComponentDesc));
	ENSURE(AddComponent(tMeshComponentDesc.m_wstrComponentName, m_spMeshComponent));
}

void Client::CTreasureBox::InitColliderComponent()
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

	m_spSphereComponent = CSphereComponent::Create(make_shared<FSphereComponentDesc>(tSphereComponentDesc));
	AddComponent(tSphereComponentDesc.m_wstrComponentName, m_spSphereComponent);
}
