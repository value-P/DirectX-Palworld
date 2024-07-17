#pragma once
namespace Engine
{
	class CLayer;
	class CWorld;
	class CGameObject;
	class CColliderComponent;

	class ENGINE_DLL CLevel : public enable_shared_from_this<CLevel>
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CLevel() = default;

		virtual ~CLevel() = default;

	/********************
		Events
	********************/
	public:
		virtual HRESULT Initialize();

		virtual HRESULT BeginPlay();
		
		virtual int32 PreTick(float _fDeltaSeconds);

		virtual int32 Tick(float _fDeltaSeconds);

		virtual int32 PostTick(float _fDeltaSeconds);

		virtual HRESULT EndPlay();

		virtual void Release();

		virtual void Clear();

	public:
		virtual void LoadResource(int32 _iLevelType);

	/********************
		Methods
	********************/
	public:
		void AddGameObject(const std::shared_ptr<CGameObject>& _spGameObject);

		void RemoveGameObject(const std::shared_ptr<CGameObject>& _spGameObject);

		std::shared_ptr<CGameObject> FindGameObjectByName(const std::wstring& _wstrGameObjectName);

		std::shared_ptr<CGameObject> FindGameObjectByID(uint32 _iGameObjectID);
		
	public:
		void RequestSimulationColliderComponent(const std::shared_ptr<CColliderComponent>& _spColliderComponent);

		void RequestTransformColliderComponent(const std::shared_ptr<CColliderComponent>& _spColliderComponent);

	private:
		void DeferredAddGameObject();

		void DeferredRemoveGameObject();

		void DeferredSimulationColliderComponents();

		void DeferredTransformColliderComponents();

	public:
		HRESULT AddGameObjectToLayer(const std::wstring& _wstrLayerName, const std::shared_ptr<CGameObject>& _spGameObject);

		HRESULT RemoveGameObjectFromLayer(const std::wstring& _wstrLayerName, const std::wstring& _wstrGameObjectName);

		std::shared_ptr<CLayer> FindLayer(const std::wstring& _wstrLayerName);

	/********************
		Getter/Setter
	********************/
	public:
		std::shared_ptr<CWorld> GetOwnerWorld() const;

		void SetOwnerWorld(const std::shared_ptr<CWorld>& _spOwnerWorld) { m_wpOwnerWorld = _spOwnerWorld; }

		void SetLevelName(const std::wstring& _wstrLevelName) { m_wstrLevelName = _wstrLevelName; }

		const std::wstring& GetLevelName() const { return m_wstrLevelName; }

		const std::unordered_map<std::wstring, std::shared_ptr<CLayer>>& GetLayers() const { return m_umapLayers; }

		const std::vector<std::shared_ptr<CGameObject>>& GetGameObjects() const { return m_vecGameObjects; }

		int32 GetLevelType() const { return m_iLevelType; }

		void SetLevelState(ELevelState _eLevelState) { m_eLevelState.store(_eLevelState); }

		ELevelState GetLevelState() const { return m_eLevelState.load(); }

	/********************
		Data Members
	********************/
	private:
		// �� ������ �ִ� ��� ���� ������Ʈ��
		std::vector<std::shared_ptr<CGameObject>> m_vecGameObjects;

		// ƽ �߿� �߰��� ���� ������Ʈ��
		std::vector<std::shared_ptr<CGameObject>> m_vecDeferredAddGameObjects;

		// ƽ �߿� ������ ���� ������Ʈ��
		std::vector<std::shared_ptr<CGameObject>> m_vecDeferredRemoveGameObjects;

		// �� ������ �ִ� ��� ���̾��
		std::unordered_map<std::wstring, std::shared_ptr<CLayer>> m_umapLayers;

		// �� ������ ���� �ִ� ����
		std::weak_ptr<CWorld> m_wpOwnerWorld;

		// �� ������ �̸�
		std::wstring m_wstrLevelName = L"";

		// ���� ���ؽ�
		std::mutex m_mtMutex;

		// �ݶ��̴� ������Ʈ ���ؽ�
		std::mutex m_mtColliderComponentMutex;

		// ���� ����
		std::atomic<ELevelState> m_eLevelState = ELevelState::INITIALIZING;

	protected:
		// ƽ �߿� Ȱ��ȭ, ��Ȱ��ȭ�� �ݶ��̴� ������Ʈ��
		std::vector<std::shared_ptr<CColliderComponent>> m_vecDeferredSimulationColliderComponents;

		// ƽ �߿� Ʈ�������� ������ �ݶ��̴� ������Ʈ��
		std::vector<std::shared_ptr<CColliderComponent>> m_vecDeferredTransformColliderComponents;

	protected:
		// ���� �ε���
		int32 m_iLevelType = -1;
	};
}
