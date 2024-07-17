#pragma once

namespace Client
{
	class CBuildManager;
	class CItem;
	class CHabitat;
	class CMonster;
	class CArchitecture;

	struct FToDoCommand final
	{
		// �ؾ� �� ��
		EToDoType m_eToDoType = EToDoType::ENUM_END;

		// �� ���� �� �� �ִ� �۾� �Ӽ�
		EMonsterTaskType m_eMonsterTaskType = EMonsterTaskType::ENUM_END;

		std::shared_ptr<CHabitat> m_spHabitat = nullptr;

		std::shared_ptr<CItem> m_spItem = nullptr;

		EToDoCommandState m_eToDoCommandState = EToDoCommandState::NOT_ASSIGNED;

		int32 m_iCommandID = -1;
	};

	/********************
		Descriptor
	********************/
	struct FToDoSystemDesc final
	{
	};

	class CToDoSystem final
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CToDoSystem() = default;

		virtual ~CToDoSystem() = default;

	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CToDoSystem> Create(const std::shared_ptr<FToDoSystemDesc>& _spToDoSystemDesc);

		HRESULT Initialize(const std::shared_ptr<FToDoSystemDesc>& _spToDoSystemDesc);
		
	public:
		HRESULT BeginPlay();

		int32 PreTick(float _fDeltaSeconds);

		int32 Tick(float _fDeltaSeconds);

		int32 PostTick(float _fDeltaSeconds);

		HRESULT EndPlay();

		void Release();

	/********************
		Methods
	********************/
	public:
		// ���� ���� ����մϴ�.
		HRESULT AddSlavePal(const std::shared_ptr<CMonster>& _spSlavePal);

		// ���� ���� �����մϴ�.
		HRESULT RemoveSlavePal(const std::shared_ptr<CMonster>& _spSlavePal);

	public:
		// ���� ���� �ؾ��� �۾��� ����մϴ�.
		int32 AddCommandToSystem(FToDoCommand& _tToDoCommand);

		HRESULT RemoveCommandFromSystem(EToDoType _eToDoType, int32 _iCommandID);

		void PauseCommandFromSlavePal(EToDoType _eToDoType);

		void RequestCommandFromSlavePal();

	private:
		void CommandToSlavePal(EToDoType _eToDoType);

		void CommandToAllSlavePal();

	public:
		std::shared_ptr<CArchitecture> FindArchitecture(EArchitectureType _eArchitectureType);

	/********************
		Getter/Setter
	********************/
	public:
		HRESULT SetCommandState(EToDoType _eToDoType, int32 _iCommandID, EToDoCommandState _eToDoCommandState);

		EToDoCommandState GetCommandState(EToDoType _eToDoType, int32 _iCommandID);

	/********************
		Data Members
	********************/
	private:
		// �������� ����Ǵ� ��ɵ�
		std::array<std::map<int32, std::shared_ptr<FToDoCommand>>, (int32)EToDoType::ENUM_END> m_arrToDoCommands;

		// �۾��� �� �ִ� �ӵ�
		std::array<std::map<int32, std::shared_ptr<CMonster>>, (int32)EMonsterTaskType::ENUM_END> m_arrSlavePals;

		// �۾� �ĺ��� ī����
		int32 m_iCommandID = 0;

	private:
		std::weak_ptr<CBuildManager> m_wpBuildManager;
	};
}
