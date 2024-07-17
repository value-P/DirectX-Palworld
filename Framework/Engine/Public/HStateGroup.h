#pragma once

namespace Engine
{
	class CGameObject;
	class CHFSMComponent;
	class CHState;

	class ENGINE_DLL CHStateGroup : public enable_shared_from_this<CHStateGroup>
	{
	public:
		struct FHStateGroupDesc
		{
			std::wstring m_wstrStateGroupName = L"";

			int32 m_iHStateGroupIndex = -1;

			std::shared_ptr<CGameObject> m_spOwnerGameObject = nullptr;
		};
		
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CHStateGroup() = default;

		virtual ~CHStateGroup() = default;
		
	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CHStateGroup> Create(const std::shared_ptr<FHStateGroupDesc>& _spHStateGroupDesc);

		virtual HRESULT Initialize(const std::shared_ptr<FHStateGroupDesc>& _spHStateGroupDesc);
		
	public:
		virtual int32 Tick(int32 _iStateIndex, float _fDeltaSeconds);

		virtual void Release();

	/********************
		Methods
	********************/
	public:
		HRESULT AddHState(const std::shared_ptr<CHState>& _spHState);

		HRESULT RemoveHState(int32 _iHStateIndex);

	/********************
		Getter/Setter
	********************/
	public:
		int32 GetHStateGroupIndex() const { return m_iHStateGroupIndex; }

	/********************
		Data Member
	********************/
	protected:
		std::shared_ptr<CGameObject> m_spOwnerGameObject = nullptr;

		std::weak_ptr<CHFSMComponent> m_wpHFSMComponent;

		std::unordered_map<int32, std::shared_ptr<CHState>> m_umapHStates;

	protected:
		std::wstring m_wstrStateGroupName = L"";

		int32 m_iHStateGroupIndex = -1;

		int32 m_iCurHStateIndex = -1;
	};
}
