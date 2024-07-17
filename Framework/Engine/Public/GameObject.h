#pragma once

namespace Engine
{
	class CComponent;
	class CTransformComponent;
	class CColliderComponent;
	class CRigidbodyComponent;

	class CWorld;
	class CLevel;
	class CLayer;
	class CTimer;

	class ENGINE_DLL CGameObject : public enable_shared_from_this<CGameObject>
	{
	public:
		struct ENGINE_DLL FGameObjectDesc
		{
			Vector3 m_vInitScale = Vector3(1.0f, 1.0f, 1.0f);

			Vector3 m_vInitRotate = Vector3(0.0f, 0.0f, 0.0f);

			Vector3 m_vInitPosition = Vector3(0.0f, 0.0f, 0.0f);

			// ���� ������Ʈ �����Ƽ
			EMobilityType m_eMobilityType = EMobilityType::STATIC;

			// ���� ������Ʈ �̸�
			std::wstring m_wstrGameObjectName = L"";

			// ���� ������Ʈ Ȱ��ȭ ����
			bool m_bActive = true;

			// ��Ʈ�� �ε���
			int32 m_iOctreeIndex = -1;

			// ��Ʈ�� ���� ��� �ε���
			std::tuple<int32, int32, int32> m_tpOctreeNodeIndex{ -1, -1, -1 };
		};

	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CGameObject() = default;

		virtual ~CGameObject() = default;
		
	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CGameObject> Create(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc);

		virtual HRESULT Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc);

	public:
		virtual HRESULT BeginPlay();

		virtual int32 PreTick(float _fDeltaSeconds);

		virtual int32 Tick(float _fDeltaSeconds);
		
		virtual int32 PostTick(float _fDeltaSeconds);

		virtual HRESULT EndPlay();

		virtual void Release();
		
	/********************
		Getter/Setter
	********************/
	public:
		const std::optional<Vector3> GetForwardVector() const;

		const std::optional<Vector3> GetUpVector() const;

		const std::optional<Vector3> GetRightVector() const;

		const std::optional<Vector3> GetForwardVectorWithoutY() const;

		const std::optional<Vector3> GetRightVectorWithoutY() const;

	public:
		void SetScale(const Vector3& _vScale);

		const std::optional<Vector3> GetScale() const;

		void SetRotate(const Vector3& _vAxis, float _fDegree);

		void SetRotate(const Vector3& _vRotate);

		void SetRotateAccumulated(const Vector3& _vAxis, float _fDegree);

		const std::optional<Vector3> GetRotate() const;

		virtual const std::optional<Quaternion> GetQuaternion() const;

		virtual void SetPosition(const Vector3& _vPosition);

		virtual const std::optional<Vector3> GetPosition() const;

		void SetTransform(ETransformType _eTransformType, const Vector4& _vTransform);

		const std::optional<Vector4> GetTransform(ETransformType _eTransformType);

		virtual void SetTransformMatrix(const Matrix& _matMatrix);

		const std::optional<Matrix> GetTransformMatrix() const;

	public:
		const std::shared_ptr<CWorld> GetOwnerWorld() const;

		const std::shared_ptr<CLevel> GetOwnerLevel() const;

		const std::shared_ptr<CGameObject> GetParentGameObject() const;

		const std::shared_ptr<CLayer> GetLayer() const;

	public:
		void SetOwnerWorld(const std::shared_ptr<CWorld>& _spWorld) { m_wpOwnerWorld = _spWorld; }

		void SetOwnerLevel(const std::shared_ptr<CLevel>& _spLevel) { m_wpOwnerLevel = _spLevel; }

		void SetParentGameObject(const std::shared_ptr<CGameObject>& _spParentGameObject) { m_wpParentGameObject = _spParentGameObject; }

		void SetLayer(const std::shared_ptr<CLayer>& _spLayer) { m_wpLayer = _spLayer; }

	public:
		void SetGameObjectName(const std::wstring& _wstrGameObjectName) { m_wstrGameObjectName = _wstrGameObjectName; }

		const std::wstring& GetGameObjectName() const { return m_wstrGameObjectName; }

	public:
		void SetCulled(bool _bCulled);

		bool IsCulled() const { return m_bCulled.load(); }

		virtual void SetActive(bool _bActive);

		bool IsActive() const { return m_bActive.load(); }

		void SetGameObjectID();

		uint32 GetGameObjectID() const { return m_iGameObjectID; }

		EMobilityType GetMobilityType() const { return m_eMobilityType; }

	public:
		// ��Ʈ�� ���� ����
		void SetOctreeNodeIndex(const std::tuple<int32, int32, int32>& _tpLeafNodeIndex) { m_tpOctreeNodeIndex = _tpLeafNodeIndex; }

		const std::tuple<int32, int32, int32>& GetOctreeNodeIndex() const { return m_tpOctreeNodeIndex; }

		void SetOctreeIndex(int32 _iOctreeIndex) { m_iOctreeIndex = _iOctreeIndex; }

		int32 GetOctreeIndex() const { return m_iOctreeIndex; }

		// ����Ʈ�� ���� ����
		void SetQuadtreeNodeIndex(const std::tuple<int32, int32>& _tpLeafNodeIndex) { m_tpQuadtreeNodeIndex = _tpLeafNodeIndex; }

		const std::tuple<int32, int32>& GetQuadtreeNodeIndex() const { return m_tpQuadtreeNodeIndex; }

		void SetQuadtreeIndex(int32 _iQuadtreeIndex) { m_iQuadtreeIndex = _iQuadtreeIndex; }

		int32 GetQuadtreeIndex() const { return m_iQuadtreeIndex; }

	/******************
		Methods
	******************/
	public:
		virtual void LookAt(const Vector3& _vPosition);

		virtual void Turn(const Vector3& _vAxis, float _fAngularSpeed);

		virtual void MoveForward(float _fSpeed);

		virtual void MoveBackward(float _fSpeed);

		virtual void MoveRight(float _fSpeed);

		virtual void MoveLeft(float _fSpeed);

		virtual void MoveUp(float _fSpeed);

		virtual void MoveDown(float _fSpeed);

	public:
		HRESULT AddComponent(const std::wstring& _wstrComponentName, const std::shared_ptr<CComponent>& _spComponent);

		void AddChildGameObject(const std::shared_ptr<CGameObject>& _spGameObject);

		template<typename T, typename = std::enable_if_t<std::is_base_of_v<CComponent, T>>>
		std::shared_ptr<T> FindComponent(const std::wstring& _wstrComponentName)
		{
			if (!m_mapComponents.contains(_wstrComponentName)) { return nullptr; }

			return std::dynamic_pointer_cast<T>(m_mapComponents[_wstrComponentName]);
		}

		HRESULT RemoveComponent(const std::wstring& _wstrComponentName);

	public:
		virtual void OnCollisionEnter(CComponent* _pSrcComponent, CComponent* _pDstComponent);

		virtual void OnCollisionStay(CComponent* _pSrcComponent, CComponent* _pDstComponent);

		virtual void OnCollisionExit(CComponent* _pSrcComponent, CComponent* _pDstComponent);

		virtual void OnTriggerEnter(CComponent* _pSrcComponent, CComponent* _pDstComponent);

		virtual void OnTriggerStay(CComponent* _pSrcComponent, CComponent* _pDstComponent);

		virtual void OnTriggerExit(CComponent* _pSrcComponent, CComponent* _pDstComponent);

		virtual void OnAnimEnter();

		virtual void OnAnimStay();

		virtual void OnAnimExit();

		virtual void AnimNotifyCall(const list<FAnimNotifyData>& _lstAnimNotify);
		
		virtual void OnTimerOver(const shared_ptr<CTimer>& _spTimer);

	/********************
		Data Members
	********************/
	protected:
		// �� ���� ������Ʈ�� ���� ����
		std::weak_ptr<CWorld> m_wpOwnerWorld;

		// �� ���� ������Ʈ�� ���� ����
		std::weak_ptr<CLevel> m_wpOwnerLevel;

		// �� ���� ������Ʈ�� ���� ���̾�
		std::weak_ptr<CLayer> m_wpLayer;

		// �� ���� ������Ʈ�� ���� ���� ������Ʈ
		std::weak_ptr<CGameObject> m_wpParentGameObject;

		// �� ���� ������Ʈ�� ���� �ִ� ������Ʈ��
		std::map<std::wstring, std::shared_ptr<CComponent>> m_mapComponents;

		// �� ���� ������Ʈ�� ���� �ִ� �ڽ� ���� ������Ʈ��
		std::vector<std::shared_ptr<CGameObject>> m_vecChildGameObjects;

	protected:
		// �� ���� ������Ʈ�� �����Ƽ
		EMobilityType m_eMobilityType = EMobilityType::STATIC;

		// �� ���� ������Ʈ�� �̸�
		std::wstring m_wstrGameObjectName = L"";

		// ���� ������Ʈ ���� ����
		std::atomic<bool> m_bCulled = false;

		// ���� ������Ʈ Ȱ��ȭ(ƽ, ����) ����
		std::atomic<bool> m_bActive = true;

		// ���� ������Ʈ Ʈ������ ������Ʈ
		std::weak_ptr<CTransformComponent> m_wpTransformComponent;

	protected:
		// �� ���� ������Ʈ�� �ĺ���
		uint32 m_iGameObjectID = 0;

	private:
		// ���� ������Ʈ�� ���� ��Ʈ�� �ε���
		int32 m_iOctreeIndex = -1;

		// ���� ������Ʈ�� ���� ��Ʈ�� ������� �ε���
		std::tuple<int32, int32, int32> m_tpOctreeNodeIndex{ -1, -1, -1 };

		// ���� ������Ʈ�� ���� ���� Ʈ�� �ε���
		int32 m_iQuadtreeIndex = -1;

		// �� ���� ������Ʈ�� ���� ����Ʈ�� ���� ��� �ε���
		std::tuple<int32, int32> m_tpQuadtreeNodeIndex{ -1, -1 };
	};
}