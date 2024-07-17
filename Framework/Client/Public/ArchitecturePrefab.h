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
		/* ī�޶� �ٶ󺸴� �������� �̵��մϴ�. */
		void FollowCameraLook();

		/* ���� �ش� �����տ� �ش��ϴ� ���๰�� ���� ������ �������� ������ üũ�մϴ�. */
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

		/* �÷� */
		void SetColor(Color _vColor) { m_vColor = _vColor; }
		const Color& GetColor() { return m_vColor; }

		/* ���� �Ÿ� */
		void SetBuildDistance(float _fBuildDistance) { m_fBuildDistance = _fBuildDistance; }
		float GetBuildDistance() { return m_fBuildDistance; }

	/********************
		Data Members
	********************/
	protected:
		/* ���� �Ŵ��� */
		std::shared_ptr<CBuildManager> m_spBuildManager = nullptr;

		/* ������ �Ŵ��� */
		std::weak_ptr<CPhysXManager> m_wpPhysXManager;

		/* ����Ʈ ���̴� */
		std::wstring m_wstrShaderName = L"";

		/* �� ���� ��� */
		std::wstring m_wstrModelFilePath = L"";

	protected:
		/* �ݶ��̴�(���� ���� ���� �Ǵܿ�) */
		std::shared_ptr<CBoxComponent> m_spBoxComponent = nullptr;
		Vector3 m_vColliderExtents = Vector3(1.f, 1.f, 1.f);
		Vector3 m_vColliderCenter = Vector3(0.f, 0.f, 0.f);
		Vector3 m_vColliderRotate = Vector3(0.f, 0.f, 0.f);

		/* �����հ� �浹 ���� ������Ʈ�� */
		std::unordered_map<uint32, std::shared_ptr<CGameObject>> m_umapCollidedGameObjects;

		/* �÷�(�ʷϻ� : ���� ����, ������ : ���� �Ұ�) */
		Color m_vColor = Color(0.f, 1.f, 0.f, 1.f);

		/* �÷��̾� */
		std::weak_ptr<CPlayer> m_wpPlayer;

		/* ���� �Ÿ� */
		float m_fBuildDistance = 10.f;

		bool m_bHousingPrefab = false;

		float m_fRotationCount = 0.f;
	};
}