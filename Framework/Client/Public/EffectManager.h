#pragma once
#include "Manager.h"

namespace Engine
{
	class CGameObject;
	class CModel;
	class CBone;
}

namespace Client
{
	class CEffectBase;
	class CParticleObject;
	class CEffectMesh;
	class CInstanceEffectMesh;
	class CEffectGroup;
	class CPalSummonParticle;
	class CPalDeadParticle;
	class CBowlingPinAppearParticle;
	class CBuildStartParticle;
	class CPalRetrieveTrail;

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

	private:
		void InitializeEffects();
		void InitializeSummonEffects();
		void InitializeDeadEffects();
		void InitializeBuildStartEffects();
		void InitializeBowlingPinAppearEffects();
		void InitializePalRetrieveTrails();

	/********************
		Methods
	********************/
	public:
		// 이펙트 추가
		HRESULT AddEffect(const std::wstring& _wstrEffectName, std::shared_ptr<CEffectGroup> _spEffectGroup);

		// 이펙트 삭제
		HRESULT RemoveEffect(const std::wstring& _wstrEffectName);

		// 이펙트 검색
		std::shared_ptr<CEffectGroup> FindEffect(const std::wstring& _wstrEffectName);

		// 이펙트 활성화
		shared_ptr<CEffectGroup> ActivateEffect(const std::wstring& _wstrEffectName, const Vector3& _vPos);
		shared_ptr<CEffectGroup> ActivateEffect(const std::wstring& _wstrEffectName, shared_ptr<CGameObject> _spObject, const Vector3& _vPos);
		shared_ptr<CEffectGroup> ActivateEffectCombined(const std::wstring& _wstrEffectName, shared_ptr<CGameObject> _spObject, const Vector3& _vOffset);
		shared_ptr<CEffectGroup> ActivateEffectCombined(const std::wstring& _wstrEffectName, shared_ptr<CGameObject> _spObject, std::shared_ptr<CBone> _spBone, const Vector3& _vOffset);

		// 파트너 팰 소환, 해제
		shared_ptr<CPalSummonParticle> ActivePalSummonParticle(std::shared_ptr<CGameObject> _spGameObject, std::shared_ptr<CModel> _spModel);
		shared_ptr<CPalRetrieveTrail> ActivePalRetrieveTrail(const Vector3& _vStartPos, const Vector3& _vEndPos, float _fTrailSize);

		// 볼링핀 파티클
		shared_ptr<CBowlingPinAppearParticle> ActiveBowlingPinParticle(std::shared_ptr<CGameObject> _spGameObject, std::shared_ptr<CModel> _spModel);

		// 팰 사망
		shared_ptr<CPalDeadParticle> ActivePalDeadParticle(std::shared_ptr<CGameObject> _spGameObject, std::shared_ptr<CModel> _spModel);

		// 건축 시작
		shared_ptr<CBuildStartParticle> ActiveBuildStartParticle(std::shared_ptr<CGameObject> _spGameObject, std::shared_ptr<CModel> _spModel);

	/********************
		Getter/Setter
	********************/
	public:
		

	/********************
		Data Members
	********************/
	private:
		std::wstring m_wstrEffectGroupPath = L"../../Resource/Effects/EffectGroup/";

	private:
		// 이펙트 컨테이너
		std::unordered_map<std::wstring, std::vector<std::shared_ptr<CEffectGroup>>> m_umapEffects;

		// 팰 소환, 회수 파티클 이펙트
		std::array<std::shared_ptr<CPalSummonParticle>, 10> m_arrPartnerPalSummonEffects;

		// 팰 사망 파티클 이펙트
		std::array<std::shared_ptr<CPalDeadParticle>, 5> m_arrPartnerPalDeadEffects;

		// 건축물 설치 시 파티클 이펙트
		std::array<std::shared_ptr<CBuildStartParticle>, 5> m_arrBuildStartEffects;

		// 볼링핀 이펙트
		std::array<std::shared_ptr<CBowlingPinAppearParticle>, 10> m_arrBowlingPinAppearEffects;

		// 팰 회수 트레일
		std::shared_ptr<CPalRetrieveTrail> m_spPalRetrieveTrail = nullptr;
	};
}
