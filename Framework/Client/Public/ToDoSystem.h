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
		// 해야 할 일
		EToDoType m_eToDoType = EToDoType::ENUM_END;

		// 이 일을 할 수 있는 작업 속성
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
		// 거점 팰을 등록합니다.
		HRESULT AddSlavePal(const std::shared_ptr<CMonster>& _spSlavePal);

		// 거점 팰을 해제합니다.
		HRESULT RemoveSlavePal(const std::shared_ptr<CMonster>& _spSlavePal);

	public:
		// 거점 팰이 해야할 작업을 등록합니다.
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
		// 거점에서 진행되는 명령들
		std::array<std::map<int32, std::shared_ptr<FToDoCommand>>, (int32)EToDoType::ENUM_END> m_arrToDoCommands;

		// 작업할 수 있는 팰들
		std::array<std::map<int32, std::shared_ptr<CMonster>>, (int32)EMonsterTaskType::ENUM_END> m_arrSlavePals;

		// 작업 식별자 카운터
		int32 m_iCommandID = 0;

	private:
		std::weak_ptr<CBuildManager> m_wpBuildManager;
	};
}
