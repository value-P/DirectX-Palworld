// Default
#include "pch.h"
#include "EffectManager.h"
#include "StringUtility.h"

// Module, Manager
#include "ModelHandler.h"

// Level
#include "Level.h"

// GameObject
#include "EffectGroup.h"

IMPLEMENT_SINGLETON(CEffectManager)

HRESULT ToolAnimation::CEffectManager::Initialize()
{
	InitializeEffects();

	return S_OK;
}

HRESULT ToolAnimation::CEffectManager::BeginPlay()
{
	for (auto& pair : m_umapEffects)
	{
		for (auto& effect : pair.second)
		{
			m_wpCurrentLevel.lock()->AddGameObject(effect);
		}
	}

	return S_OK;
}

void ToolAnimation::CEffectManager::Release()
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

	m_spInstance.reset();
	m_spInstance = nullptr;
}

void ToolAnimation::CEffectManager::InitializeEffects()
{
	for (const filesystem::directory_entry& fsEntry : filesystem::recursive_directory_iterator(m_wstrEffectGroupPath))
	{
		// 파일이 아닌 경우
		if (!filesystem::is_regular_file(fsEntry)) { continue; }

		wstring wstrJsonFileName = fsEntry.path().stem().generic_wstring();

		/* FEffectGroupDesc */
		FEffectGroupDesc tEffectGroupDesc;

		tEffectGroupDesc.m_bActive = false;
		tEffectGroupDesc.m_wstrEffectGroupName = wstrJsonFileName;

		vector<shared_ptr<CEffectGroup>> vecEffectGroup = {};
		m_umapEffects.emplace(wstrJsonFileName, vecEffectGroup);
		for (uint32 innerIndex = 0; innerIndex < 5; ++innerIndex)
		{
			/* JSON파일로부터 객체를 생성합니다. */
			shared_ptr<CEffectGroup> spEffectGroup = CEffectGroup::CreateFromJson(make_shared<FEffectGroupDesc>(tEffectGroupDesc), wstrJsonFileName);
			CHECKF(spEffectGroup, L"Failed to CreateFromJson : CEffectManager");

			m_umapEffects[wstrJsonFileName].push_back(spEffectGroup);
		}

		string strEffectName = CStringUtility::ToString(wstrJsonFileName);
		m_umapEffectNames.emplace(strEffectName, wstrJsonFileName);
		m_lstEffectStrNames.push_back(strEffectName);
	}
}

HRESULT ToolAnimation::CEffectManager::ActivateEffect(const string& _strEffectName, const Vector3& _vPos)
{
	wstring wstrEffectName = m_umapEffectNames[_strEffectName];

	if (!m_umapEffects.contains(wstrEffectName)) { return E_FAIL; }

	for (auto& effect : m_umapEffects[wstrEffectName])
	{
		if (!effect->IsActive())
		{
			effect->SetPosition(_vPos);
			effect->Reset();
			return S_OK;
		}
	}

	return E_FAIL;
}

HRESULT ToolAnimation::CEffectManager::ActivateEffectCombined(const std::string& _strEffectName, shared_ptr<CGameObject> _spObject, shared_ptr<CBone> _spBone, const Vector3& _vOffset)
{
	wstring wstrEffectName = m_umapEffectNames[_strEffectName];

	if (!m_umapEffects.contains(wstrEffectName)) { return E_FAIL; }

	for (auto& effect : m_umapEffects[wstrEffectName])
	{
		if (!effect->IsActive())
		{
			effect->SetCombinedOffset(_vOffset);
			effect->SetCombinedBone(_spBone);
			effect->SetCombinedGameObject(_spObject);

			effect->Reset();
			return S_OK;
		}
	}

	return E_FAIL;
}
