#pragma once

namespace Client
{
	class CInventorySystem;
	class CBuildManager;
	class CPalBoxSystem;

	struct FHUDTutorialInfo;

	enum class EQuestType
	{
		QUEST_ITEM,
		QUEST_PAL,
		QUEST_BUILDING,
		QUEST_CUSTOM,
		ENUM_END
	};

	struct FQuest
	{
		EQuestType m_eQuestType = EQuestType::ENUM_END;

		EArchitectureType m_eDstBuildingType = EArchitectureType::ENUM_END;
		EItemType m_eDstItemType = EItemType::NONE;
		EMonsterType m_eDstMonsterType = EMonsterType::ENUM_END;

		int32 m_iDstObjectCount = 1;
		wstring m_wstrQuestText = L"";
		bool m_bIsCleared = false;
	};

	struct FQuestGroup
	{
		array<FQuest, 3> m_arrQuest;
	};

	struct FQuestSystemDesc
	{
		unordered_map<std::wstring, FQuestGroup> m_umapQuestSheet;
	};

	class CQuestSystem final
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CQuestSystem() = default;

		virtual ~CQuestSystem() = default;
		
	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CQuestSystem> Create(const std::shared_ptr<FQuestSystemDesc>& _spQuestSystemDesc);

		HRESULT Initialize(const std::shared_ptr<FQuestSystemDesc>& _spQuestSystemDesc);

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
		void AddQuest(const wstring& _wstrQuestName, const FQuestGroup& _tQuest);
		bool ActiveQuest(const wstring& _wstrQuestName);
		bool ClearCurrentQuest();
		bool IsCurrentQuestCleared();
		bool IsCurrentQuest(const wstring& _wstrQuestName) { return m_wstrCurrentQuest == _wstrQuestName; }
		bool IsCurrentQuestEmpty() { return m_wstrCurrentQuest == L""; }
		
		void SetCurrentQuestTrue(int32 _iQuestIndex);

	private:
		void ComputeFinishedQuest();

	/********************
		Data Members
	********************/
	private:
		weak_ptr<CBuildManager> m_wpBuildManager;
		weak_ptr<CInventorySystem> m_wpInventorySystem;
		weak_ptr<CPalBoxSystem> m_wpPalboxSystem;

		weak_ptr<FHUDTutorialInfo> m_wpQuestUI;
						
		unordered_map<std::wstring, FQuestGroup> m_umapQuestSheet;
		wstring m_wstrCurrentQuest = L"";
	};
}


