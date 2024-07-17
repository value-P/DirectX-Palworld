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
		// 이 레벨에 있는 모든 게임 오브젝트들
		std::vector<std::shared_ptr<CGameObject>> m_vecGameObjects;

		// 틱 중에 추가할 게임 오브젝트들
		std::vector<std::shared_ptr<CGameObject>> m_vecDeferredAddGameObjects;

		// 틱 중에 삭제할 게임 오브젝트들
		std::vector<std::shared_ptr<CGameObject>> m_vecDeferredRemoveGameObjects;

		// 이 레벨에 있는 모든 레이어들
		std::unordered_map<std::wstring, std::shared_ptr<CLayer>> m_umapLayers;

		// 이 레벨을 갖고 있는 월드
		std::weak_ptr<CWorld> m_wpOwnerWorld;

		// 이 레벨의 이름
		std::wstring m_wstrLevelName = L"";

		// 레벨 뮤텍스
		std::mutex m_mtMutex;

		// 콜라이더 컴포넌트 뮤텍스
		std::mutex m_mtColliderComponentMutex;

		// 레벨 상태
		std::atomic<ELevelState> m_eLevelState = ELevelState::INITIALIZING;

	protected:
		// 틱 중에 활성화, 비활성화할 콜라이더 컴포넌트들
		std::vector<std::shared_ptr<CColliderComponent>> m_vecDeferredSimulationColliderComponents;

		// 틱 중에 트랜스폼을 변경할 콜라이더 컴포넌트들
		std::vector<std::shared_ptr<CColliderComponent>> m_vecDeferredTransformColliderComponents;

	protected:
		// 레벨 인덱스
		int32 m_iLevelType = -1;
	};
}
