#pragma once

namespace Engine
{
	class CCameraManager;
	class CRenderManager;
	class CGameObject;
	class CLevel;

	class ENGINE_DLL CTreeNode : public enable_shared_from_this<CTreeNode>
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CTreeNode() = default;

		virtual ~CTreeNode() = default;
		
	/********************
		Events
	********************/
	public:
		virtual HRESULT Initialize(int32 _iTreeIndex, uint32 _iNodeDepth, const Vector3& _vCenter, const Vector3& _vExtents);

		virtual void Tick(float _fDeltaSeconds);
		
		virtual void Render();

		virtual void Release();
		
	/********************
		Methods
	********************/
	public:
		void CullGameObject(const BoundingFrustum& _tFrustum, const BoundingSphere& _tSphere);

		HRESULT AddGameObjectToNode(const std::shared_ptr<CGameObject>& _spGameObject);

		HRESULT RemoveGameObjectFromNodeByID(EMobilityType _eMobilityType, int32 _iGameObjectID);

		std::shared_ptr<CGameObject> FindGameObjectFromNode(EMobilityType _eMobilityType, int32 _iGameObjectID);

	/********************
		Getter/Setter
	********************/
	public:
		Vector3 GetExtents() const { return m_tDxAABB.Extents; }

		Vector3 GetCenter() const { return m_tDxAABB.Center; }

	/********************
		Data Members
	********************/
	protected:
		// �� ��尡 ���� �ִ� �ڽ� ����
		std::vector<std::shared_ptr<CTreeNode>> m_vecChildNodes;

		// �� ����� ����
		int32 m_iNodeDepth = 0;

		// �� ����� �ε���
		int32 m_iNodeIndex = -1;

		// ���� ������� ����
		bool m_bLeaf = false;

	protected:
		// �� ��� �ȿ� �ִ� ������ ���� ������Ʈ��
		std::unordered_map<int32, std::shared_ptr<CGameObject>> m_umapStaticGameObjects;

		// �� ��� �ȿ� �ִ� ������ ���� ������Ʈ��
		std::unordered_map<int32, std::shared_ptr<CGameObject>> m_umapDynamicGameObjects;

	protected:
		// DX AABB �ڽ�
		BoundingBox m_tDxAABB{};

		ComPtr<ID3D11DeviceContext> m_cpDeviceContext = nullptr;

		std::shared_ptr<PrimitiveBatch<VertexPositionColor>> m_spPrimitiveBatch = nullptr;

		std::shared_ptr<BasicEffect> m_spBasicEffect = nullptr;

		ComPtr<ID3D11InputLayout> m_cpInputLayout = nullptr;

		bool m_bCollided = false;

	protected:
		std::weak_ptr<CCameraManager> m_wpCameraManager;

		std::weak_ptr<CRenderManager> m_wpRenderManager;

		std::weak_ptr<CLevel> m_wpCurLevel;

		std::mutex m_mtMutex;
	};
}
