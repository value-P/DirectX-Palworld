#pragma once

namespace Engine
{
	class CGameObject;

	class ENGINE_DLL CLayer : public enable_shared_from_this<CLayer>
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CLayer() = default;

		virtual ~CLayer() = default;
		
	/********************
		Events
	********************/
	public:
		static shared_ptr<CLayer> Create(const std::wstring& _wstrLayerName);

		HRESULT Initialize(const std::wstring& _wstrLayerName);

		void Release();
		
	/********************
		Methods
	********************/
	public:
		HRESULT AddGameObjectToLayer(const std::wstring& _wstrGameObjectName, const std::shared_ptr<CGameObject>& _spGameObject);

		HRESULT RemoveGameObjectFromLayer(const std::wstring& _wstrGameObjectName);

		std::shared_ptr<CGameObject> FindGameObjectFromLayer(const std::wstring& _wstrGameObjectName);

	/********************
		Getter / Setter
	********************/
	public:
		const std::unordered_map<std::wstring, std::shared_ptr<CGameObject>>& GetGameObjects() const { return m_umapGameObjects; }

		const std::wstring& GetLayerName() { return m_wstrLayerName; }

	/********************
		Data Members
	********************/
	private:
		// 레이어 이름
		std::wstring m_wstrLayerName = L"";

		// 이 레이어에 속한 게임 오브젝트들
		std::unordered_map<std::wstring, std::shared_ptr<CGameObject>> m_umapGameObjects;
	};
}
