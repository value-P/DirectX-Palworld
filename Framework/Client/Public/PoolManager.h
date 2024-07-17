#pragma once
#include "Manager.h"

namespace Engine
{
	class CGameObject;
}

namespace Client
{
	class CBerryGoat;
	class CChickenPal;
	class CDreamDemon;
	class CHedgeHog;
	class CKitsunebi;
	class CPenguin;
	class CPinkCat;
	class CSheepBall;
	class CGrassMammoth;

	class CMineral;
	class CIngredient;

	class CPoolManager : public CManager
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		DECLARE_SINGLETON(CPoolManager)
		
	/********************
		Events
	********************/
	public:
		HRESULT Initialize();

		HRESULT BeginPlay();

		void Release();
		
	private:
		void InitPal();

		void InitMineral();

		void InitIngredient();

	/********************
		Methods
	********************/
	public:
		template<typename T>
		std::shared_ptr<T> PopPalFromPool(EMonsterType _eMonsterType)
		{

		}

		template<typename T>
		std::shared_ptr<T> PopMineralFromPool(EMineralType _eMineralType)
		{
		}

		template<typename T>
		std::shared_ptr<T> PopIngredientFromPool(EIngredientType _eIngredientType)
		{

		}

	public:
		void PushPalFromPool();

		void PushMineralFromPool();

		void PushIngredientFromPool();

	/********************
		Data Members
	********************/
	private:
		int32 m_iNumPals = 10;

	private:
		std::vector<std::shared_ptr<CBerryGoat>> m_vecBerryGoats;

		std::vector<std::shared_ptr<CChickenPal>> m_vecChickenPals;

		std::vector<std::shared_ptr<CDreamDemon>> m_vecDreamDemons;

		std::vector<std::shared_ptr<CHedgeHog>> m_vecHedgeHogs;

		std::vector<std::shared_ptr<CKitsunebi>> m_vecKitsunebis;

		std::vector<std::shared_ptr<CPenguin>> m_vecPenguins;

		std::vector<std::shared_ptr<CPinkCat>> m_vecPinkCats;

		std::vector<std::shared_ptr<CSheepBall>> m_vecSheepBalls;

		std::vector<std::shared_ptr<CGrassMammoth>> m_vecGrassMammoths;

	private:
		std::vector<std::shared_ptr<CMineral>> m_vecMinerals[(int32)EMineralType::ENUM_END];

		std::vector<std::shared_ptr<CIngredient>> m_vecIngredients[(int32)EIngredientType::ENUM_END];
	};
}
