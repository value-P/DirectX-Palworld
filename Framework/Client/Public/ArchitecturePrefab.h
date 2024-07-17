#pragma once
#include "Architecture.h"

namespace Engine
{
	class CPhysXManager;
	class CMeshComponent;
	class CBoxComponent;
}

namespace Client
{
	class CPlayer;
	class CBuildManager;

	struct FArchitecturePrefabDesc : public FArchitectureDesc
	{
		std::wstring m_wstrShaderName = L"";

		std::wstring m_wstrModelFilePath = L"";

		std::shared_ptr<CPlayer> m_spPlayer;

		Vector3 m_vColliderExtents = Vector3(1.f, 1.f, 1.f);

		Vector3 m_vColliderCenter = Vector3(0.f, 0.f, 0.f);

		Vector3 m_vColliderRotate = Vector3(0.f, 0.f, 0.f);
	};

	class CArchitecturePrefab : public CArchitecture
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CArchitecturePrefab() = default;

		virtual ~CArchitecturePrefab() = default;
		
	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CArchitecturePrefab> Create(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc);

		virtual HRESULT Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc) override;

		virtual HRESULT BeginPlay() override;

		virtual int32 PreTick(float _fDeltaSeconds) override;

		virtual int32 Tick(float _fDeltaSeconds) override;
		
		virtual int32 PostTick(float _fDeltaSeconds) override;

		virtual HRESULT EndPlay() override;

		virtual void Release() override;

	public:
		virtual void RecycleFromPool() override;
		virtual void ReturnToPool() override;

	public:
		virtual void OnCollisionEnter(CColliderComponent* _pSrcColliderComponent, CColliderComponent* _pDstColliderComponent);
		virtual void OnCollisionStay(CColliderComponent* _pSrcColliderComponent, CColliderComponent* _pDstColliderComponent);
		virtual void OnCollisionExit(CColliderComponent* _pSrcColliderComponent, CColliderComponent* _pDstColliderComponent);

	private:
		virtual HRESULT InitializeMeshComponent() override;

		virtual HRESULT InitializeColliderComponent() override;

	/********************
		Methods
	********************/
	public:
		/* 카메라가 바라보는 방향으로 이동합니다. */
		void FollowCameraLook();

		/* 현재 해당 프리팹에 해당하는 건축물이 건축 가능한 상태인지 조건을 체크합니다. */
		virtual bool IsBuildable();

	/********************
		Getter/Setter
	********************/
	public:
		const std::wstring& GetShaderName() { return m_wstrShaderName; }
		const std::wstring& GetModelFilePath() { return m_wstrModelFilePath; }
		const Vector3& GetColliderExtents() { return m_vColliderExtents; }
		const Vector3& GetColliderCenter() { return m_vColliderCenter; }
		const Vector3& GetColliderRotate() { return m_vColliderRotate; }

		/* 컬러 */
		void SetColor(Color _vColor) { m_vColor = _vColor; }
		const Color& GetColor() { return m_vColor; }

		/* 건축 거리 */
		void SetBuildDistance(float _fBuildDistance) { m_fBuildDistance = _fBuildDistance; }
		float GetBuildDistance() { return m_fBuildDistance; }

	/********************
		Data Members
	********************/
	protected:
		/* 빌드 매니저 */
		std::shared_ptr<CBuildManager> m_spBuildManager = nullptr;

		/* 피직스 매니저 */
		std::weak_ptr<CPhysXManager> m_wpPhysXManager;

		/* 이펙트 셰이더 */
		std::wstring m_wstrShaderName = L"";

		/* 모델 파일 경로 */
		std::wstring m_wstrModelFilePath = L"";

	protected:
		/* 콜라이더(건축 가능 여부 판단용) */
		std::shared_ptr<CBoxComponent> m_spBoxComponent = nullptr;
		Vector3 m_vColliderExtents = Vector3(1.f, 1.f, 1.f);
		Vector3 m_vColliderCenter = Vector3(0.f, 0.f, 0.f);
		Vector3 m_vColliderRotate = Vector3(0.f, 0.f, 0.f);

		/* 프리팹과 충돌 중인 오브젝트들 */
		std::unordered_map<uint32, std::shared_ptr<CGameObject>> m_umapCollidedGameObjects;

		/* 컬러(초록색 : 건축 가능, 빨간색 : 건축 불가) */
		Color m_vColor = Color(0.f, 1.f, 0.f, 1.f);

		/* 플레이어 */
		std::weak_ptr<CPlayer> m_wpPlayer;

		/* 건축 거리 */
		float m_fBuildDistance = 10.f;

		bool m_bHousingPrefab = false;

		float m_fRotationCount = 0.f;
	};
}