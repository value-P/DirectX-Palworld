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
		// ����Ʈ �߰�
		HRESULT AddEffect(const std::wstring& _wstrEffectName, std::shared_ptr<CEffectGroup> _spEffectGroup);

		// ����Ʈ ����
		HRESULT RemoveEffect(const std::wstring& _wstrEffectName);

		// ����Ʈ �˻�
		std::shared_ptr<CEffectGroup> FindEffect(const std::wstring& _wstrEffectName);

		// ����Ʈ Ȱ��ȭ
		shared_ptr<CEffectGroup> ActivateEffect(const std::wstring& _wstrEffectName, const Vector3& _vPos);
		shared_ptr<CEffectGroup> ActivateEffect(const std::wstring& _wstrEffectName, shared_ptr<CGameObject> _spObject, const Vector3& _vPos);
		shared_ptr<CEffectGroup> ActivateEffectCombined(const std::wstring& _wstrEffectName, shared_ptr<CGameObject> _spObject, const Vector3& _vOffset);
		shared_ptr<CEffectGroup> ActivateEffectCombined(const std::wstring& _wstrEffectName, shared_ptr<CGameObject> _spObject, std::shared_ptr<CBone> _spBone, const Vector3& _vOffset);

		// ��Ʈ�� �� ��ȯ, ����
		shared_ptr<CPalSummonParticle> ActivePalSummonParticle(std::shared_ptr<CGameObject> _spGameObject, std::shared_ptr<CModel> _spModel);
		shared_ptr<CPalRetrieveTrail> ActivePalRetrieveTrail(const Vector3& _vStartPos, const Vector3& _vEndPos, float _fTrailSize);

		// ������ ��ƼŬ
		shared_ptr<CBowlingPinAppearParticle> ActiveBowlingPinParticle(std::shared_ptr<CGameObject> _spGameObject, std::shared_ptr<CModel> _spModel);

		// �� ���
		shared_ptr<CPalDeadParticle> ActivePalDeadParticle(std::shared_ptr<CGameObject> _spGameObject, std::shared_ptr<CModel> _spModel);

		// ���� ����
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
		// ����Ʈ �����̳�
		std::unordered_map<std::wstring, std::vector<std::shared_ptr<CEffectGroup>>> m_umapEffects;

		// �� ��ȯ, ȸ�� ��ƼŬ ����Ʈ
		std::array<std::shared_ptr<CPalSummonParticle>, 10> m_arrPartnerPalSummonEffects;

		// �� ��� ��ƼŬ ����Ʈ
		std::array<std::shared_ptr<CPalDeadParticle>, 5> m_arrPartnerPalDeadEffects;

		// ���๰ ��ġ �� ��ƼŬ ����Ʈ
		std::array<std::shared_ptr<CBuildStartParticle>, 5> m_arrBuildStartEffects;

		// ������ ����Ʈ
		std::array<std::shared_ptr<CBowlingPinAppearParticle>, 10> m_arrBowlingPinAppearEffects;

		// �� ȸ�� Ʈ����
		std::shared_ptr<CPalRetrieveTrail> m_spPalRetrieveTrail = nullptr;
	};
}
