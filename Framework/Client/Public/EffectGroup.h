#pragma once
#include "GameObject.h"

namespace Engine
{
	class CSoundManager;

	class CBone;
	class CSphereComponent;
	class CBoxComponent;
}

namespace Client
{
	struct FColliderInfo
	{
		bool m_bCollider = false;

		EColliderType m_eColliderType = EColliderType::ENUM_END;
		ECollisionFlag m_eCollisionFlag = ECollisionFlag::ENUM_END;
		ECollisionLayer m_eCollisionLayer = ECollisionLayer::ENUM_END;

		// Sphere
		float m_fRadius = 0.f;

		// Box
		Vector3 m_vExtents = Vector3(0.f);
		Vector3 m_vLocalPosition = Vector3(0.f);
		Vector3 m_vLocalRotate = Vector3(0.f);
	};
}

namespace Client
{
	class CEffectBase;

	struct FEffectGroupDesc : public CGameObject::FGameObjectDesc
	{
		std::wstring m_wstrEffectGroupName = L"";
		float m_fDuration = 0.f;
	};

	class CEffectGroup : public CGameObject
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CEffectGroup() = default;
		virtual ~CEffectGroup() = default;
		
	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CEffectGroup> Create(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc);
		static std::shared_ptr<CEffectGroup> CreateFromJson(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc, const std::wstring& _wstrJsonFileName);

	public:
		virtual HRESULT Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc);
		HRESULT InitializeFromJson(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc, const std::wstring& _wstrJsonFileName);

		virtual HRESULT BeginPlay() override;

		virtual int32 PreTick(float _fDeltaSeconds) override;

		virtual int32 Tick(float _fDeltaSeconds) override;

		virtual int32 PostTick(float _fDeltaSeconds) override;

		virtual HRESULT EndPlay() override;

		virtual void Release() override;
		
	public:
		HRESULT InitializeCollider(FColliderInfo _tColliderInfo);

	public:
		virtual void OnTriggerEnter(CComponent* _pSrcComponent, CComponent* _pDstComponent);
		virtual void OnTriggerExit(CComponent* _pSrcComponent, CComponent* _pDstComponent);

	/********************
		Methods
	********************/
	public:
		HRESULT LoadFromJsonFile(const std::wstring& _wstrJsonFileName);

		FParticleInfo		MakeParticleInfoFromJson(rapidjson::Value& _Value);
		FEffectMeshInfo		MakeEffectMeshInfoFromJson(rapidjson::Value& _Value);
		FInstanceEffectMeshInfo	MakeInstanceEffectMeshInfoFromJson(rapidjson::Value& _Value);

	public:
		/* ����Ʈ �׷쿡 ����Ʈ�� �߰� */
		HRESULT AddEffect(const std::wstring& _wstrEffectName, std::shared_ptr<CEffectBase> _spEffect);

		/* ����Ʈ �׷쿡�� ����Ʈ�� ���� */
		HRESULT RemoveEffect(const std::wstring& _wstrEffectName);

		/* ����Ʈ �׷쿡�� ����Ʈ�� �˻� */
		std::shared_ptr<CEffectBase> FindEffect(const std::wstring& _wstrEffectName);

		/* ���յ� ������ ��� ���� */
		void AttachToBone();

		/* (����)�ݶ��̴� ���� ������Ʈ */
		void UpdateColliderInfo(FColliderInfo _tColliderInfo);

	public:
		/* ���� */
		virtual void Reset();

		/* ������Ʈ Ǯ���� ���� �� �Ҹ� �Լ� */
		virtual void SetCull(bool _bCulled);

		/* ��Ȱ��ȭ �� ���� */
		virtual void NoneActivate();

		/* ���� �� ���� */
		virtual void EndLifeTime();

		/* ���� �� ���� */
		virtual void Loop();

	/********************
		Getter / Setter
	********************/
	public:
		/* ����Ʈ ���� */
		const std::map<std::wstring, std::shared_ptr<CEffectBase>>& GetEffects() { return m_mapEffects; }

	public:
		/* ����Ʈ ���� �̸� */
		void SetEffectGroupName(const std::wstring& _wstrEffectGroupName) { m_wstrEffectGroupName = _wstrEffectGroupName; }
		const std::wstring& GetEffectGroupName() { return m_wstrEffectGroupName; }

		/* �Ͻ� ���� */
		void SetPause() { m_bPause = !m_bPause; }
		bool GetPause() { return m_bPause; }

		/* �෹�̼� */
		void SetDuration(float _fDuration) { m_fDuration = _fDuration; }
		float GetDuration() { return m_fDuration; }
		
		/* Ʈ�������� */
		void SetTrackPosition(float _fTrackPosition) { m_fTrackPosition = _fTrackPosition; }
		float GetTrackPosition() { return m_fTrackPosition; }

		/* ���� */
		void SetLoop() { m_bLoop = !m_bLoop; }
		bool GetLoop() { return m_bLoop; }

		/* �ݶ��̴� */
		const FColliderInfo& GetColliderInfo() { return m_tColliderInfo; }
		void SetCollisionOption(ECollisionLayer _eCollisionLayer, ECollisionFlag _eCollisionFlag);

		/* ���� */
		const std::wstring& GetSoundName() { return m_wstrSoundName; }
		float GetSoundValue() { return m_fSoundValue; }

	public:
		void SetCombinedOffset(const Vector3& _vCombinedOffset) { m_vCombinedOffset = _vCombinedOffset; }
		void SetCombinedBone(std::shared_ptr<CBone> _spCombinedBond) { m_spCombinedBone = _spCombinedBond; }
		void SetCombinedGameObject(std::shared_ptr<CGameObject> _spCombinedObject) { m_spCombinedGameObject = _spCombinedObject; }

	/********************
		Data Members
	********************/
	protected:
		/* ����Ʈ ���� �̸� */
		std::wstring m_wstrEffectGroupName = L"";

		/* ����Ʈ ���� */
		std::map<std::wstring, std::shared_ptr<CEffectBase>> m_mapEffects;

	protected:
		/* ����Ʈ ��ü ���� �ð� */
		float m_fDuration = 0.f;

		/* ����Ʈ Ʈ�������� */
		float m_fTrackPosition = 0.f;

		/* ���� */
		bool m_bLoop = false;

		/* �Ͻ� ���� */
		bool m_bPause = false;

		/* �� */
		bool m_bFinished = false;

	protected:
		/* ����Ʈ�� ������ ������Ʈ */
		std::shared_ptr<CGameObject> m_spCombinedGameObject = nullptr;

		/* ����Ʈ�� ������ �� */
		std::shared_ptr<CBone> m_spCombinedBone = nullptr;

		/* ���� ������ */
		Vector3 m_vCombinedOffset = Vector3(0.f);

	private:
		std::weak_ptr<CSoundManager> m_wpSoundManager;

		std::wstring m_wstrSoundName = L"";

		EChannelType m_eChannel = EChannelType::ENUM_END;

		bool m_bSoundLoop = false;

		float m_fSoundValue = 1.f;

	protected:
		std::shared_ptr<CSphereComponent> m_spSphereComponent = nullptr;
		std::shared_ptr<CBoxComponent> m_spBoxComponent = nullptr;

		FColliderInfo m_tColliderInfo = {};
	};
}