#pragma once
#include "Manager.h"

namespace Engine
{
	class CGameObject;
	class CLevel;
	class CBone;
}

namespace ToolAnimation
{
	class CEffectBase;
	class CParticleObject;
	class CEffectMesh;
	class CInstanceEffectMesh;
	class CEffectGroup;

	class CEffectManager final : public CManager
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		DECLARE_SINGLETON(CEffectManager)

	/********************
		Events
	********************/
	public:
		HRESULT Initialize();
		HRESULT BeginPlay();
		virtual void Release() override;

		void SetCurrentLevel(shared_ptr<CLevel> _spLevel) { m_wpCurrentLevel = _spLevel; }

	private:
		void InitializeEffects();

	/********************
		Methods
	********************/
	public:		
		/* 이펙트 활성화 */
		HRESULT ActivateEffect(const std::string& _strEffectName, const Vector3& _vPos);
		HRESULT ActivateEffectCombined(const std::string& _strEffectName, shared_ptr<CGameObject> _spObject, shared_ptr<CBone> _spBone, const Vector3& _vOffset);

	/********************
		Getter/Setter
	********************/
	public:
		const std::list<std::string>& const GetEffectNames() { return m_lstEffectStrNames; }

	/********************
		Data Members
	********************/
	private:
		std::wstring m_wstrEffectGroupPath = L"../../Resource/Effects/EffectGroup/";

		weak_ptr<CLevel> m_wpCurrentLevel;

	private:
		/* 이펙트 컨테이너 */
		std::unordered_map<std::wstring, std::vector<std::shared_ptr<CEffectGroup>>> m_umapEffects;
		std::unordered_map<std::string, std::wstring> m_umapEffectNames;
		std::list<std::string> m_lstEffectStrNames;
	};
}