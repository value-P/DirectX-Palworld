#pragma once

namespace Engine
{
	class CTimerManager;
	class CTimer;
	class CPanel;
}

namespace Client
{
	class CUIManager;
	class CGoldCoin;
	class CTreasureBox;

	/********************
		Descriptor
	********************/
	struct FCollectingCoinSystemDesc final
	{
	};

	class CCollectingCoinSystem final
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CCollectingCoinSystem() = default;

		virtual ~CCollectingCoinSystem() = default;

	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CCollectingCoinSystem> Create(const std::shared_ptr<FCollectingCoinSystemDesc >& _spCollectingCoinSystemDesc);

		HRESULT Initialize(const std::shared_ptr<FCollectingCoinSystemDesc >& _spCollectingCoinSystemDesc);

		HRESULT BeginPlay();

		int32 PreTick(float _fDeltaSeconds);

		int32 Tick(float _fDeltaSeconds);
		
		int32 PostTick(float _fDeltaSeconds);

		HRESULT EndPlay();

		void Release();

	/********************
		Getter/Setter
	********************/
	public:

	/********************
		Methods
	********************/
	public:
		// 시간 카운팅함수
		void TimeAttackTimer();

	/********************
		Data Members
	********************/
	private:
		std::weak_ptr<CTimerManager>   m_wpTimerManager;

		std::weak_ptr<CUIManager>	   m_wpUIManager;

		std::vector<std::shared_ptr<CGoldCoin>> m_vecGoldCoins;
									  
		std::shared_ptr<CGoldCoin>	   m_spGoldCoin    = nullptr;
			
		std::shared_ptr<CTreasureBox>  m_spTreasureBox = nullptr;

		std::shared_ptr<CPanel>		   m_spParentPanel = nullptr;

		std::shared_ptr<CTimer>		   m_spTimer = nullptr;
	};
}