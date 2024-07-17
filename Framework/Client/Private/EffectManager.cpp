// Default
#include "pch.h"
#include "EffectManager.h"

// Module, Manager
#include "ModelHandler.h"
#include "ClientModule.h"
#include "GameManager.h"

// Level
#include "World.h"
#include "Level.h"

// GameObject
#include "EffectGroup.h"
#include "PalSummonParticle.h"
#include "PalDeadParticle.h"
#include "BuildStartParticle.h"
#include "BowlingPinAppearParticle.h"
#include "PalRetrieveTrail.h"

IMPLEMENT_SINGLETON(CEffectManager)

HRESULT Client::CEffectManager::Initialize()
{
	shared_ptr<CLevel> spPersistentLevel = CWorld::GetInstance()->GetPersistentLevel();

	InitializeEffects();
	InitializeSummonEffects();
	InitializeDeadEffects();
	InitializeBuildStartEffects();
	// InitializeBowlingPinAppearEffects();
	InitializePalRetrieveTrails();

	for (auto& pair : m_umapEffects)
	{
		for (auto& spEffect : pair.second)
		{
			spPersistentLevel->AddGameObject(spEffect);
		}
	}
	for (auto& palsummoneffect : m_arrPartnerPalSummonEffects)
	{
		spPersistentLevel->AddGameObject(palsummoneffect);
	}
	for (auto& palDeadeffect : m_arrPartnerPalDeadEffects)
	{
		spPersistentLevel->AddGameObject(palDeadeffect);
	}
	//for (auto& bowlingpineffect : m_arrBowlingPinAppearEffects)
	//{
	//	spPersistentLevel->AddGameObject(bowlingpineffect);
	//}
	for (auto& buildstarteffect : m_arrBuildStartEffects)
	{
		spPersistentLevel->AddGameObject(buildstarteffect);
	}

	spPersistentLevel->AddGameObject(m_spPalRetrieveTrail);

	return S_OK;
}

HRESULT Client::CEffectManager::BeginPlay()
{
	return S_OK;
}

void Client::CEffectManager::Release()
{
	for (auto& pair : m_umapEffects)
	{
		for (auto& effect : pair.second)
		{
			if (nullptr != effect)
			{
				effect->Release();
				effect = nullptr;
			}
		}
		pair.second.clear();
	}
	m_umapEffects.clear();

	for (auto& effect : m_arrPartnerPalSummonEffects)
	{
		effect = nullptr;
	}
	for (auto& effect : m_arrPartnerPalDeadEffects)
	{
		effect = nullptr;
	}
	for (auto& effect : m_arrBuildStartEffects)
	{
		effect = nullptr;
	}

	m_spPalRetrieveTrail = nullptr;

	m_spInstance.reset();
	m_spInstance = nullptr;
}

void Client::CEffectManager::InitializeEffects()
{
	for (const filesystem::directory_entry& fsEntry : filesystem::recursive_directory_iterator(m_wstrEffectGroupPath))
	{
		// 파일이 아닌 경우
		if (!filesystem::is_regular_file(fsEntry)) { continue; }

		FEffectGroupDesc tEffectGroupDesc;

		tEffectGroupDesc.m_bActive = false;
		tEffectGroupDesc.m_wstrEffectGroupName = fsEntry.path().stem().generic_wstring();

		vector<shared_ptr<CEffectGroup>> vecEffectGroup = {};
		m_umapEffects.emplace(tEffectGroupDesc.m_wstrEffectGroupName, vecEffectGroup);
		for (uint32 innerIndex = 0; innerIndex < 10; ++innerIndex)
		{
			/* JSON파일로부터 객체를 생성합니다. */
			shared_ptr<CEffectGroup> spEffectGroup = CEffectGroup::CreateFromJson(make_shared<FEffectGroupDesc>(tEffectGroupDesc), tEffectGroupDesc.m_wstrEffectGroupName);
			CHECKF(spEffectGroup, L"Failed to CreateFromJson : CEffectManager");
			spEffectGroup->SetPosition(Vector3(0.f, -1000.f, 1000.f));

			m_umapEffects[tEffectGroupDesc.m_wstrEffectGroupName].push_back(spEffectGroup);
		}
	}
}

void Client::CEffectManager::InitializeSummonEffects()
{
	FPalSummonParticleDesc tPalSummonParticleDesc;

	tPalSummonParticleDesc.m_bActive = false;
	tPalSummonParticleDesc.m_wstrEffectGroupName = L"Group_PartnerPal_Summon";

	for(uint16 i = 0; i < 10; ++i)
	{
		/* JSON파일로부터 객체를 생성합니다. */
		shared_ptr<CPalSummonParticle> spPalSummonParticle = CPalSummonParticle::CreateFromJson(make_shared<FPalSummonParticleDesc>(tPalSummonParticleDesc), tPalSummonParticleDesc.m_wstrEffectGroupName);
		CHECKF(spPalSummonParticle, L"Failed to CreateFromJson : CEffectManager");

		m_arrPartnerPalSummonEffects[i] = spPalSummonParticle;
	}
}

void Client::CEffectManager::InitializeDeadEffects()
{
	FPalDeadParticleDesc tPalDeadParticleDesc;

	tPalDeadParticleDesc.m_bActive = false;
	tPalDeadParticleDesc.m_wstrEffectGroupName = L"Group_PalDead";

	for (uint16 i = 0; i < 5; ++i)
	{
		/* JSON파일로부터 객체를 생성합니다. */
		shared_ptr<CPalDeadParticle> spPalDeadParticle = CPalDeadParticle::CreateFromJson(make_shared<FPalDeadParticleDesc>(tPalDeadParticleDesc), tPalDeadParticleDesc.m_wstrEffectGroupName);
		CHECKF(spPalDeadParticle, L"Failed to CreateFromJson : CEffectManager");

		m_arrPartnerPalDeadEffects[i] = spPalDeadParticle;
	}
}

void Client::CEffectManager::InitializeBuildStartEffects()
{
	FBuildStartParticleDesc tBuildStartParticleDesc;

	tBuildStartParticleDesc.m_bActive = false;
	tBuildStartParticleDesc.m_wstrEffectGroupName = L"Group_BuildStart";

	for (uint16 i = 0; i < 5; ++i)
	{
		/* JSON파일로부터 객체를 생성합니다. */
		shared_ptr<CBuildStartParticle> spBuildStartParticle = CBuildStartParticle::CreateFromJson(make_shared<FBuildStartParticleDesc>(tBuildStartParticleDesc), tBuildStartParticleDesc.m_wstrEffectGroupName);
		CHECKF(spBuildStartParticle, L"Failed to CreateFromJson : CEffectManager");

		m_arrBuildStartEffects[i] = spBuildStartParticle;
	}
}

void Client::CEffectManager::InitializeBowlingPinAppearEffects()
{
	FBowlingPinAppearParticleDesc tBowlingPinAppearParticleDesc;

	tBowlingPinAppearParticleDesc.m_bActive = false;
	tBowlingPinAppearParticleDesc.m_wstrEffectGroupName = L"Group_BowlingPin_Appear";

	for (uint16 i = 0; i < 10; ++i)
	{
		/* JSON파일로부터 객체를 생성합니다. */
		shared_ptr<CBowlingPinAppearParticle> spBowlingPinAppearParticle = CBowlingPinAppearParticle::CreateFromJson(make_shared<FBowlingPinAppearParticleDesc>(tBowlingPinAppearParticleDesc), tBowlingPinAppearParticleDesc.m_wstrEffectGroupName);
		CHECKF(spBowlingPinAppearParticle, L"Failed to CreateFromJson : CEffectManager");

		m_arrBowlingPinAppearEffects[i] = spBowlingPinAppearParticle;
	}
}

void Client::CEffectManager::InitializePalRetrieveTrails()
{
	FPalRetrieveTrailDesc tPalRetrieveTrailDesc;

	tPalRetrieveTrailDesc.m_bActive = false;

	m_spPalRetrieveTrail = CPalRetrieveTrail::Create(make_shared<FPalRetrieveTrailDesc>(tPalRetrieveTrailDesc));
	CHECKF(m_spPalRetrieveTrail, L"Failed to Create CPalRetrieveTrail : CEffectManager")
}

HRESULT Client::CEffectManager::AddEffect(const wstring& _wstrEffectName, shared_ptr<CEffectGroup> _spEffectGroup)
{
	if (m_umapEffects.contains(_wstrEffectName)) { return E_FAIL; }

	m_umapEffects[_wstrEffectName].push_back(_spEffectGroup);

	return S_OK;
}

HRESULT Client::CEffectManager::RemoveEffect(const wstring& _wstrEffectName)
{
	if (!m_umapEffects.contains(_wstrEffectName)) { return E_FAIL; }

	for (auto& effect : m_umapEffects[_wstrEffectName])
	{
		effect->Release();
		effect = nullptr;
	}
	m_umapEffects[_wstrEffectName].clear();
	m_umapEffects.erase(_wstrEffectName);

	return S_OK;
}

std::shared_ptr<CEffectGroup> Client::CEffectManager::FindEffect(const std::wstring& _wstrEffectName)
{
	if (!m_umapEffects.contains(_wstrEffectName)) { return nullptr; }

	// 활성화 되어있지 않은 이펙트를 찾아서 리턴합니다.
	for (auto& effect : m_umapEffects[_wstrEffectName])
	{
		if (!effect->IsActive())
		{
			return effect;
		}
	}

	// 검색되지 않았다면 nullptr을 리턴합니다.
	return nullptr;
}

shared_ptr<CEffectGroup> Client::CEffectManager::ActivateEffect(const wstring& _wstrEffectName, const Vector3& _vPos)
{
	if (!m_umapEffects.contains(_wstrEffectName)) { return nullptr; }

	for (auto& effect : m_umapEffects[_wstrEffectName])
	{
		if (!effect->IsActive())
		{
			effect->SetPosition(_vPos);
			effect->Reset();
			return effect;
		}
	}

	return nullptr;
}

shared_ptr<CEffectGroup> Client::CEffectManager::ActivateEffect(const std::wstring& _wstrEffectName, shared_ptr<CGameObject> _spObject, const Vector3& _vPos)
{
	if (!m_umapEffects.contains(_wstrEffectName)) { return nullptr; }

	for (auto& effect : m_umapEffects[_wstrEffectName])
	{
		if (!effect->IsActive())
		{
			effect->SetPosition(_vPos);

			Vector3 vPos = effect->GetPosition().value();
			Vector3 vLook = _spObject->GetForwardVector().value();
			vLook.y = 0.f;
			vLook.Normalize();

			effect->SetPosition(_vPos);
			effect->LookAt(vPos + vLook);

			effect->Reset();
			return effect;
		}
	}

	return nullptr;
}

shared_ptr<CEffectGroup> Client::CEffectManager::ActivateEffectCombined(const std::wstring& _wstrEffectName, shared_ptr<CGameObject> _spObject, const Vector3& _vOffset)
{
	if (!m_umapEffects.contains(_wstrEffectName)) { return nullptr; }

	for (auto& effect : m_umapEffects[_wstrEffectName])
	{
		if (!effect->IsActive())
		{
			effect->SetCombinedOffset(_vOffset);
			effect->SetCombinedGameObject(_spObject);

			effect->Reset();
			return effect;
		}
	}

	return nullptr;
}

shared_ptr<CEffectGroup> Client::CEffectManager::ActivateEffectCombined(const std::wstring& _wstrEffectName, shared_ptr<CGameObject> _spObject, shared_ptr<CBone> _spBone, const Vector3& _vOffset)
{
	if (!m_umapEffects.contains(_wstrEffectName)) { return nullptr; }

	for (auto& effect : m_umapEffects[_wstrEffectName])
	{
		if (!effect->IsActive())
		{
			effect->SetCombinedOffset(_vOffset);
			effect->SetCombinedBone(_spBone);
			effect->SetCombinedGameObject(_spObject);

			effect->Reset();
			return effect;
		}
	}

	return nullptr;
}

shared_ptr<CPalSummonParticle> Client::CEffectManager::ActivePalSummonParticle(std::shared_ptr<CGameObject> _spGameObject, std::shared_ptr<CModel> _spModel)
{
	for (auto& palsummoneffect : m_arrPartnerPalSummonEffects)
	{
		if (!palsummoneffect->IsActive())
		{
			palsummoneffect->SetPosition(_spGameObject->GetPosition().value());
			palsummoneffect->ResetAtBonePosition(_spGameObject, _spModel);
			return palsummoneffect;
		}
	}

	return nullptr;
}

shared_ptr<CPalRetrieveTrail> Client::CEffectManager::ActivePalRetrieveTrail(const Vector3& _vStartPos, const Vector3& _vEndPos, float _fTrailSize)
{
	if (!m_spPalRetrieveTrail) { return nullptr; }

	if (!m_spPalRetrieveTrail->IsActive())
	{
		m_spPalRetrieveTrail->SetPosition(_vStartPos);
		m_spPalRetrieveTrail->SetActive(true);
		m_spPalRetrieveTrail->OnTrail(_vStartPos, _vEndPos, _fTrailSize);
	}

	return nullptr;
}

shared_ptr<CBowlingPinAppearParticle> Client::CEffectManager::ActiveBowlingPinParticle(std::shared_ptr<CGameObject> _spGameObject, std::shared_ptr<CModel> _spModel)
{
	for (auto& bowlingpineffect : m_arrBowlingPinAppearEffects)
	{
		if (!bowlingpineffect->IsActive())
		{
			bowlingpineffect->SetPosition(_spGameObject->GetPosition().value());
			bowlingpineffect->ResetAtBonePosition(_spGameObject, _spModel);
			return bowlingpineffect;
		}
	}

	return nullptr;
}

shared_ptr<CPalDeadParticle> Client::CEffectManager::ActivePalDeadParticle(std::shared_ptr<CGameObject> _spGameObject, std::shared_ptr<CModel> _spModel)
{
	for (auto& palDeadeffect : m_arrPartnerPalDeadEffects)
	{
		if (!palDeadeffect->IsActive())
		{
			palDeadeffect->SetPosition(_spGameObject->GetPosition().value());
			palDeadeffect->ResetAtBonePosition(_spGameObject, _spModel);
			return palDeadeffect;
		}
	}

	return nullptr;
}

shared_ptr<CBuildStartParticle> Client::CEffectManager::ActiveBuildStartParticle(std::shared_ptr<CGameObject> _spGameObject, std::shared_ptr<CModel> _spModel)
{
	for (auto& buildstarteffect : m_arrBuildStartEffects)
	{
		if (!buildstarteffect->IsActive())
		{
			buildstarteffect->SetPosition(_spGameObject->GetPosition().value());
			buildstarteffect->ResetAtBonePosition(_spGameObject, _spModel);
			return buildstarteffect;
		}
	}

	return nullptr;
}
