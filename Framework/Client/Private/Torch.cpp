#include "pch.h"
#include "Torch.h"
#include "MathUtility.h"

#include "EngineModule.h"
#include "LightManager.h"
#include "EffectManager.h"

#include "EffectGroup.h"

#include "Bone.h"

std::shared_ptr<CTorch> Client::CTorch::Create(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	shared_ptr<CTorch> spInstance = make_shared<CTorch>();
	CHECKF(spInstance, L"CTorch : Create Failed");
	spInstance->Initialize(_spGameObjectDesc);

	return spInstance;
}

HRESULT Client::CTorch::Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	if (FAILED(CWeapon::Initialize(_spGameObjectDesc))) { return E_FAIL; }

	// manager
	m_wpLightManager = CEngineModule::GetInstance()->GetLightManager();
	m_wpEffectManager = CEffectManager::GetInstance();

	// light
	// FPointLight tPointLight;
	// ZeroMemory(&tPointLight, sizeof(FPointLight));

	// tPointLight.vPosition = Vector3(0.f, 0.f, 0.f);
	// tPointLight.fIntensity = 1.f;
	// tPointLight.vColor = Vector3(1.f, 1.f, 0.f);
	// tPointLight.fRadius = 5.f;

	// m_spPointLight = make_shared<FPointLight>(tPointLight);
	// m_wpLightManager.lock()->AddLight(L"Player Torch", m_spPointLight, true);

	return S_OK;
}

HRESULT Client::CTorch::BeginPlay()
{
	if (FAILED(CWeapon::BeginPlay())) { return E_FAIL; }

	return S_OK;
}

int32 Client::CTorch::PreTick(float _fDeltaSeconds)
{
	CWeapon::PreTick(_fDeltaSeconds);

	return 0;
}

int32 Client::CTorch::Tick(float _fDeltaSeconds)
{
	CWeapon::Tick(_fDeltaSeconds);

	Matrix matFinal;

	if (m_spFireEffect)
	{
		Matrix matOffset = Matrix::CreateTranslation(Vector3(0.f, 0.f, -47.5f));
		Matrix matWeapon = GetTransformMatrix().value();
		Matrix matBone = m_wpAttachedBone.lock()->GetCombinedTransformationMatrix();
		Matrix matOwner = m_wpParentGameObject.lock()->GetTransformMatrix().value();

		Matrix matFinal = matOffset * matWeapon * matBone * matOwner;

		m_spFireEffect->SetPosition(matFinal.Translation());
		// m_spFireEffect->SetTransformMatrix(matFinal);
	}

	if (m_spPointLight)
	{
		m_spPointLight->vPosition = matFinal.Translation();
	}

	return 0;
}

int32 Client::CTorch::PostTick(float _fDeltaSeconds)
{
	CWeapon::PostTick(_fDeltaSeconds);

	return 0;
}

HRESULT Client::CTorch::EndPlay()
{
	CWeapon::BeginPlay();

	return S_OK;
}

void Client::CTorch::Release()
{
	m_spFireEffect = nullptr;
	m_spPointLight = nullptr;

	CWeapon::Release();
}

void Client::CTorch::SetActive(bool _bActive)
{
	CWeapon::SetActive(_bActive);

	if (_bActive)
	{
		m_spFireEffect = m_wpEffectManager.lock()->ActivateEffect(L"Group_Torch", Vector3(0.f, 1.f, 0.f));
	}
	else
	{
		if (!m_spFireEffect) { return; }

		m_spFireEffect->EndLifeTime();
	}
}
