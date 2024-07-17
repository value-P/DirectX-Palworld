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
		/* 이펙트 그룹에 이펙트를 추가 */
		HRESULT AddEffect(const std::wstring& _wstrEffectName, std::shared_ptr<CEffectBase> _spEffect);

		/* 이펙트 그룹에서 이펙트를 삭제 */
		HRESULT RemoveEffect(const std::wstring& _wstrEffectName);

		/* 이펙트 그룹에서 이펙트를 검색 */
		std::shared_ptr<CEffectBase> FindEffect(const std::wstring& _wstrEffectName);

		/* 결합된 본으로 행렬 갱신 */
		void AttachToBone();

		/* (툴용)콜라이더 정보 업데이트 */
		void UpdateColliderInfo(FColliderInfo _tColliderInfo);

	public:
		/* 리셋 */
		virtual void Reset();

		/* 오브젝트 풀에서 꺼낼 때 불릴 함수 */
		virtual void SetCull(bool _bCulled);

		/* 비활성화 시 실행 */
		virtual void NoneActivate();

		/* 종료 시 실행 */
		virtual void EndLifeTime();

		/* 루프 시 실행 */
		virtual void Loop();

	/********************
		Getter / Setter
	********************/
	public:
		/* 이펙트 묶음 */
		const std::map<std::wstring, std::shared_ptr<CEffectBase>>& GetEffects() { return m_mapEffects; }

	public:
		/* 이펙트 묶음 이름 */
		void SetEffectGroupName(const std::wstring& _wstrEffectGroupName) { m_wstrEffectGroupName = _wstrEffectGroupName; }
		const std::wstring& GetEffectGroupName() { return m_wstrEffectGroupName; }

		/* 일시 정지 */
		void SetPause() { m_bPause = !m_bPause; }
		bool GetPause() { return m_bPause; }

		/* 듀레이션 */
		void SetDuration(float _fDuration) { m_fDuration = _fDuration; }
		float GetDuration() { return m_fDuration; }
		
		/* 트랙포지션 */
		void SetTrackPosition(float _fTrackPosition) { m_fTrackPosition = _fTrackPosition; }
		float GetTrackPosition() { return m_fTrackPosition; }

		/* 루프 */
		void SetLoop() { m_bLoop = !m_bLoop; }
		bool GetLoop() { return m_bLoop; }

		/* 콜라이더 */
		const FColliderInfo& GetColliderInfo() { return m_tColliderInfo; }
		void SetCollisionOption(ECollisionLayer _eCollisionLayer, ECollisionFlag _eCollisionFlag);

		/* 사운드 */
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
		/* 이펙트 묶음 이름 */
		std::wstring m_wstrEffectGroupName = L"";

		/* 이펙트 묶음 */
		std::map<std::wstring, std::shared_ptr<CEffectBase>> m_mapEffects;

	protected:
		/* 이펙트 전체 진행 시간 */
		float m_fDuration = 0.f;

		/* 이펙트 트랙포지션 */
		float m_fTrackPosition = 0.f;

		/* 루프 */
		bool m_bLoop = false;

		/* 일시 정지 */
		bool m_bPause = false;

		/* 끝 */
		bool m_bFinished = false;

	protected:
		/* 이펙트가 부착될 오브젝트 */
		std::shared_ptr<CGameObject> m_spCombinedGameObject = nullptr;

		/* 이펙트가 부착될 본 */
		std::shared_ptr<CBone> m_spCombinedBone = nullptr;

		/* 부착 오프셋 */
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