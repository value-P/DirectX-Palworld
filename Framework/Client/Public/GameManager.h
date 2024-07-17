#pragma once
#include "Manager.h"

namespace Engine
{
	class CTimerManager;
	class CLevel;
	class CTimer;
	class CDate;
}

namespace Client
{
	class CArchitectureSystem;
	class CCombatSystem;
	class CInventorySystem;
	class CMerchantSystem;
	class CPalBoxSystem;
	class CTechTreeSystem;
	class CToDoSystem;
	class CChestBoxSystem;
	class CProductionSystem;
	class CQuestSystem;
	class CPalInteractionSystem;
	class CCollectingCoinSystem;
	class CBowlingSystem;
	class CPlayer;
	
	class CGameManager final : public CManager
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		DECLARE_SINGLETON(CGameManager)

	/********************
		Events
	********************/
	public:
		HRESULT Initialize();

		HRESULT PostInitialize();

		HRESULT BeginPlay();

		int32 PreTick(float _fDeltaSeconds);

		int32 Tick(float _fDeltaSeconds);

		int32 PostTick(float _fDeltaSeconds);

		void Release();
		
	private:
		void InitQuestSystem();

	/********************
		Getter/Setter
	********************/
	public:
		std::shared_ptr<CLevel> GetCurLevel() { return m_spCurLevel; }

		void SetCurLevel(std::shared_ptr<CLevel> _spLevel) { m_spCurLevel = _spLevel; }

		std::shared_ptr<CPlayer> GetPlayer() { return m_spPlayer; }

		void SetPlayer(std::shared_ptr<CPlayer> _spPlayer) { m_spPlayer = _spPlayer; }

		float GetDeltaSeconds() const { return m_fDeltaSeconds; }

	public:
		std::shared_ptr<CInventorySystem> GetInventorySystem() { return m_spInventorySystem; }

		std::shared_ptr<CChestBoxSystem> GetChestBoxSystem() { return m_spChestBoxSystem; }

		std::shared_ptr<CPalBoxSystem> GetPalBoxSystem() { return m_spPalBoxSystem; }

		std::shared_ptr<CMerchantSystem> GetMerchantSystem() { return m_spMerchantSystem; }

		std::shared_ptr<CTechTreeSystem> GetTechTreeSystem() { return m_spTechTreeSystem; }

		std::shared_ptr<CArchitectureSystem> GetArchitectureSystem() { return m_spArchitectureSystem; }

		std::shared_ptr<CToDoSystem> GetToDoSystem() { return m_spToDoSystem; }

		std::shared_ptr<CCombatSystem> GetCombatSystem() { return m_spCombatSystem; }

		std::shared_ptr<CProductionSystem> GetProductionSystem() { return m_spProductionSystem; }

		std::shared_ptr<CQuestSystem> GetQuestSystem() { return m_spQuestSystem; }

		std::shared_ptr<CPalInteractionSystem> GetPalInteractionSystem() { return m_spPalInteractionSystem; }

		std::shared_ptr<CBowlingSystem> GetBowlingSystem() { return m_spBowlingSystem; }

		std::shared_ptr<CDate> GetDate() { return m_spDate; }

	/********************
		Data Members
	********************/
	private:
		float m_fDeltaSeconds = 0.0f;

	private:
		// 영구 레벨
		std::shared_ptr<CLevel> m_spPersistentLevel = nullptr;

		// 현재 레벨
		std::shared_ptr<CLevel> m_spCurLevel = nullptr;

	private:
		std::shared_ptr<CTimerManager> m_spTimerManager = nullptr;

		std::shared_ptr<CDate> m_spDate = nullptr;

		std::shared_ptr<CPlayer> m_spPlayer = nullptr;

	private:
		std::shared_ptr<CArchitectureSystem>	m_spArchitectureSystem	 = nullptr;
																		 
		std::shared_ptr<CCombatSystem>			m_spCombatSystem		 = nullptr;
																		 
		std::shared_ptr<CInventorySystem>		m_spInventorySystem		 = nullptr;
																		 
		std::shared_ptr<CMerchantSystem>		m_spMerchantSystem		 = nullptr;
																		 
		std::shared_ptr<CPalBoxSystem>			m_spPalBoxSystem		 = nullptr;
																		 
		std::shared_ptr<CTechTreeSystem>		m_spTechTreeSystem		 = nullptr;
																		 
		std::shared_ptr<CToDoSystem>			m_spToDoSystem			 = nullptr;
																		 
		std::shared_ptr<CChestBoxSystem>		m_spChestBoxSystem		 = nullptr;
																		 
		std::shared_ptr<CProductionSystem>		m_spProductionSystem	 = nullptr;
																		 
		std::shared_ptr<CQuestSystem>			m_spQuestSystem			 = nullptr;

		std::shared_ptr<CPalInteractionSystem>	m_spPalInteractionSystem = nullptr;

		std::shared_ptr<CCollectingCoinSystem>	m_spCollectingCoinSystem = nullptr;

		std::shared_ptr<CBowlingSystem>			m_spBowlingSystem		 = nullptr;
	};
}
