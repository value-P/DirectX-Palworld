#pragma once
#include "Level.h"

namespace Engine
{
	class CPanel;
	class CAnimationImageComponent;
}

namespace Client
{
	class CElecPanda;
	class CPlayer;
	class CRanch;
	class CGameManager;

	class CIslandLevel final : public CLevel
	{
	/********************
		Events
	********************/
	public:
		virtual HRESULT Initialize() override;

		virtual HRESULT BeginPlay() override;
		
		virtual int32 PreTick(float _fDeltaSeconds) override;
		
		virtual int32 Tick(float _fDeltaSeconds) override;

		virtual int32 PostTick(float _fDeltaSeconds) override;

		virtual void Release() override;
		
	/********************
		Methods
	********************/
	public:
		void CreateLight();

		void CreateNPC();

		void CreateEnvironment();

		void CreateLandScape();

		void CreatePlayer();

		void CreatePals();

		void CreateBowlingAlley();

		void CreateHut();

		void CreateHutArchitectures();

		void CreateMarker();

		void CreateTower();

		void InitGetItemUI();

	/********************
		Data Members
	********************/
	private:
		std::weak_ptr<CGameManager> m_wpGameManager;

	private:
		std::shared_ptr<CPlayer> m_spPlayer = nullptr;

		std::shared_ptr<CElecPanda> m_spElecPanda = nullptr;

		std::shared_ptr<CRanch> m_spRanch = nullptr;


		// GetItem 패널의 부모를 들고 있게합니다.
		std::weak_ptr<CPanel>	m_wpGetItemParentPanel;

		std::weak_ptr<CAnimationImageComponent>	m_wpGetItemAnimationImageComponentSlot0;

		std::weak_ptr<CAnimationImageComponent>	m_wpGetItemAnimationImageComponentSlot1;
	};
}
