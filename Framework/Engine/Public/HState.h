#pragma once

namespace Engine
{
	class CGameObject;
	class CHFSMComponent;
	class CHStateGroup;

	struct FHStateDesc
	{
		std::wstring m_wstrStateName = L"";

		int32 m_iHStateIndex = -1;

		std::shared_ptr<CGameObject> m_spOwnerGameObject = nullptr;
	};

	class ENGINE_DLL CHState abstract : public enable_shared_from_this<CHState>
	{
	/********************
		Events
	********************/
	public:
		virtual HRESULT Initialize(const std::shared_ptr<FHStateDesc>& _spHStateDesc);

	public:
		virtual int32 Tick(float _fDeltaSeconds);

		virtual void Release();

	/********************
		Methods
	********************/
	public:
		virtual HRESULT ResetHState() = 0;

		virtual HRESULT ReserveCommand(const std::vector<std::tuple<int32, int32>>& _vecCommand, EHFSMReserveCommandType _eHFSMReserveCommandType);

		virtual HRESULT EvaluateHState() = 0;
		
		virtual HRESULT ReconstructHState() = 0;

	/********************
		Getter/Setter
	********************/
	public:
		int32 GetHStateIndex() const { return m_iHStateIndex; }

	/********************
		Data Member
	********************/
	protected:
		std::shared_ptr<CGameObject> m_spOwnerGameObject = nullptr;

		std::weak_ptr<CHFSMComponent> m_wpHFSMComponent;

		std::weak_ptr<CHStateGroup> m_wpOwnerHStateGroup;

	protected:
		std::wstring m_wstrStateName = L"";

		int32 m_iHStateIndex = -1;
	};
}
