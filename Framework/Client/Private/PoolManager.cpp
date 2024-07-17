#include "pch.h"
#include "PoolManager.h"

#include "BerryGoat.h"
#include "ChickenPal.h"
#include "SheepBall.h"
#include "HedgeHog.h"
#include "DreamDemon.h"
#include "Penguin.h"
#include "PinkCat.h"
#include "Kitsunebi.h"
#include "GrassMammoth.h"

IMPLEMENT_SINGLETON(CPoolManager)

HRESULT Client::CPoolManager::Initialize()
{
	InitPal();
	InitMineral();
	InitIngredient();
	return S_OK;
}

HRESULT Client::CPoolManager::BeginPlay()
{
	return S_OK;
}

void Client::CPoolManager::Release()
{
	for (auto& spPal : m_vecBerryGoats) { spPal = nullptr; }
	m_vecBerryGoats.clear();

	for (auto& spPal : m_vecChickenPals) { spPal = nullptr; }
	m_vecChickenPals.clear();

	for (auto& spPal : m_vecDreamDemons) { spPal = nullptr; }
	m_vecDreamDemons.clear();

	for (auto& spPal : m_vecHedgeHogs) { spPal = nullptr; }
	m_vecHedgeHogs.clear();

	for (auto& spPal : m_vecKitsunebis) { spPal = nullptr; }
	m_vecKitsunebis.clear();

	for (auto& spPal : m_vecKitsunebis) { spPal = nullptr; }
	m_vecKitsunebis.clear();

	for (auto& spPal : m_vecPenguins) { spPal = nullptr; }
	m_vecPenguins.clear();

	for (auto& spPal : m_vecPinkCats) { spPal = nullptr; }
	m_vecPinkCats.clear();

	for (auto& spPal : m_vecSheepBalls) { spPal = nullptr; }
	m_vecSheepBalls.clear();

	for (auto& spPal : m_vecGrassMammoths) { spPal = nullptr; }
	m_vecGrassMammoths.clear();

	for (int32 iIndex = 0; iIndex < (int32)EMineralType::ENUM_END; ++iIndex)
	{
		for (auto& spMineral : m_vecMinerals[iIndex]) { spMineral = nullptr; }
		m_vecMinerals[iIndex].clear();
	}

	for (int32 iIndex = 0; iIndex < (int32)EIngredientType::ENUM_END; ++iIndex)
	{
		for (auto& spIngredient : m_vecIngredients[iIndex]) { spIngredient = nullptr; }
		m_vecIngredients[iIndex].clear();
	}
}

void Client::CPoolManager::InitPal()
{
	for (int32 iIndex = 0; iIndex < m_iNumPals; iIndex++)
	{

	}
}

void Client::CPoolManager::InitMineral()
{
}

void Client::CPoolManager::InitIngredient()
{
}
