// default
#include "pch.h"
#include "MathUtility.h"
#include "PalRetrieveTrail.h"

// manager
#include "GameManager.h"
#include "EffectManager.h"

// gameobject
#include "Player.h"

// component
#include "MeshComponent.h"
#include "TrailComponent.h"

// etc
#include "Bone.h"

shared_ptr<CPalRetrieveTrail> Client::CPalRetrieveTrail::Create(const shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	shared_ptr<CPalRetrieveTrail> spInstance = make_shared<CPalRetrieveTrail>();
	CHECKF(spInstance, L"Failed to Create : CPalRetrieveTrail");

	spInstance->Initialize(_spGameObjectDesc);

	return spInstance;
}

HRESULT Client::CPalRetrieveTrail::Initialize(const shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	if (FAILED(CGameObject::Initialize(_spGameObjectDesc))) { return E_FAIL; }

	// manager
	m_wpEffectManager = CEffectManager::GetInstance();

	InitializeTrailComponent();

	return S_OK;
}

HRESULT Client::CPalRetrieveTrail::BeginPlay()
{
	if (FAILED(CGameObject::BeginPlay())) { return E_FAIL; }

	m_wpPlayer = CGameManager::GetInstance()->GetPlayer();
	m_wpPlayerHandBone = CGameManager::GetInstance()->GetPlayer()->GetMeshComponent()->GetBonePtr(L"Socket_Weapon_R");

	return S_OK;
}

int32 Client::CPalRetrieveTrail::PreTick(float _fDeltaSeconds)
{
	switch (CGameObject::PreTick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return 0;
}

int32 Client::CPalRetrieveTrail::Tick(float _fDeltaSeconds)
{
	switch (CGameObject::Tick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	Matrix matPlayer = m_wpPlayer.lock()->GetTransformMatrix().value();
	{
		Matrix matFinal = m_wpPlayerHandBone.lock()->GetCombinedTransformationMatrix() * matPlayer;

		m_vEndPos = matFinal.Translation();
	}

	if (m_bActiveTrail) { TickTrail(0.1f, _fDeltaSeconds); }

	return 0;
}

int32 Client::CPalRetrieveTrail::PostTick(float _fDeltaSeconds)
{
	switch (CGameObject::PostTick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return 0;
}

HRESULT Client::CPalRetrieveTrail::EndPlay()
{
	if (FAILED(CGameObject::EndPlay())) { return E_FAIL; }

	return S_OK;
}

void Client::CPalRetrieveTrail::Release()
{
	m_spTrailCom = nullptr;

	CGameObject::Release();
}

HRESULT Client::CPalRetrieveTrail::InitializeTrailComponent()
{
	// trail component
	FTrailComponentDesc tTrailComponentDesc;

	tTrailComponentDesc.m_wstrComponentName = L"TrailComponent";
	tTrailComponentDesc.m_eRenderType = ERenderType::RENDER_GLOW;
	tTrailComponentDesc.m_eComponentType = EComponentType::PARTICLE;
	tTrailComponentDesc.m_wpOwnerGameObject = shared_from_this();

	tTrailComponentDesc.m_iNumRects = 20;
	tTrailComponentDesc.m_fMaxLifeTime = 0.001f;
	tTrailComponentDesc.m_vColor = Color(0.6f, 0.9f, 1.0f, 1.f);

	tTrailComponentDesc.m_wstrEffectShaderName = L"FX_Trail_PT2";

	tTrailComponentDesc.m_bUseDiffuse = false;
	tTrailComponentDesc.m_bUseNoise = false;
	tTrailComponentDesc.m_bUseMask = true;
	tTrailComponentDesc.m_bUseDistortion = false;
	tTrailComponentDesc.m_bUseDissolve = false;

	tTrailComponentDesc.m_wstrDiffuseTextureName = L"";
	tTrailComponentDesc.m_wstrNoiseTextureName = L"";
	tTrailComponentDesc.m_wstrMaskTextureName = L"T_VFX_tk_Barrier_deceive_01";
	tTrailComponentDesc.m_wstrDistortionTextureName = L"";
	tTrailComponentDesc.m_wstrDissolveTextureName = L"";

	m_spTrailCom = CTrailComponent::Create(make_shared<FTrailComponentDesc>(tTrailComponentDesc));
	CHECKF(m_spTrailCom, L"Failed to Create CTrailComponent : CPalRetrieveTrail");
	AddComponent(L"TrailComponent", m_spTrailCom);

	return S_OK;
}

void Client::CPalRetrieveTrail::TickTrail(float _fTrailSize, float _fDeltaSeconds)
{
	m_fTrailActiveAcc += _fDeltaSeconds;
	if (0.5f <= m_fTrailActiveAcc)
	{
		m_fTrailActiveAcc = 0.f;
		m_bActiveTrail = false;
		SetActive(false);
		return;
	}

	Vector3 vLerpedPos = CMathUtility::EaseOutCirc(m_vStartPos, m_vEndPos, m_fTrailActiveAcc / 0.5f);
	SetPosition(vLerpedPos);

	m_spTrailCom->TickTrail(GetPosition().value() - Vector3(0.f, m_fTrailSize, 0.f), GetPosition().value() + Vector3(0.f, m_fTrailSize, 0.f));
}

void Client::CPalRetrieveTrail::OnTrail(const Vector3& _vStartPos, const Vector3 _vEndPos, float _fTrailSize)
{
	m_vStartPos = _vStartPos;
	m_vEndPos = _vEndPos;
	m_fTrailSize = _fTrailSize;

	m_spTrailCom->ClearRectList(_vStartPos, _vStartPos);

	m_bActiveTrail = true;
}

void Client::CPalRetrieveTrail::OffTrail()
{
	m_bActiveTrail = false;
}
