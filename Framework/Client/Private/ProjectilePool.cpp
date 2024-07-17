#include "pch.h"
#include "ProjectilePool.h"

#include "Level.h"

#include "PalSphere.h"
#include "Arrow.h"
#include "PartnerPalsphere.h"
#include "RidingPalSphere.h"

shared_ptr<CProjectilePool> Client::CProjectilePool::Create(const shared_ptr<CLevel> _spLevel, EProjectileType _eProjType, const shared_ptr<FProjectileDesc>& _spInitDesc, int32 _iSize)
{
	shared_ptr<CProjectilePool> spInstance = make_shared<CProjectilePool>();
	spInstance->Initialize(_spLevel, _eProjType, _spInitDesc, _iSize);
	return spInstance;
}

void Client::CProjectilePool::Initialize(const shared_ptr<CLevel> _spLevel, EProjectileType _eProjType, const shared_ptr<FProjectileDesc>& _spInitDesc, int32 _iSize)
{
	m_spInitialDesc = _spInitDesc;
	m_wpLevel = _spLevel;

	switch (_eProjType)
	{
	case Client::EProjectileType::DEFAULT:
	{
		for (int32 i = 0; i < _iSize; ++i)
		{
			shared_ptr<CProjectile> spProj = CProjectile::Create(m_spInitialDesc);
			m_wpLevel.lock()->AddGameObject(spProj);
			spProj->SetActive(false);
			m_lstProjectilePool.push_back(spProj);
		}
	}
	break;

	case Client::EProjectileType::PARTNER_PALSPHERE:
	{
		for (int32 i = 0; i < _iSize; ++i)
		{
			shared_ptr<CProjectile> spProj = CPartnerPalsphere::Create(m_spInitialDesc);
			m_wpLevel.lock()->AddGameObject(spProj);
			spProj->SetActive(false);
			m_lstProjectilePool.push_back(spProj);
		}
	}
	break;

	case Client::EProjectileType::PALSPHERE:
	{
		for (int32 i = 0; i < _iSize; ++i)
		{
			shared_ptr<CProjectile> spProj = CPalSphere::Create(m_spInitialDesc);
			m_wpLevel.lock()->AddGameObject(spProj);
			spProj->SetActive(false);
			m_lstProjectilePool.push_back(spProj);
		}
	}
	break;

	case Client::EProjectileType::RIDING_PALSPHERE:
	{
		for (int32 i = 0; i < _iSize; ++i)
		{
			shared_ptr<CProjectile> spProj = CRidingPalSphere::Create(m_spInitialDesc);
			m_wpLevel.lock()->AddGameObject(spProj);
			spProj->SetActive(false);
			m_lstProjectilePool.push_back(spProj);
		}
	}
	break;

	case Client::EProjectileType::ARROW:
	{
		for (int32 i = 0; i < _iSize; ++i)
		{
			shared_ptr<CProjectile> spProj = CArrow::Create(m_spInitialDesc);
			m_wpLevel.lock()->AddGameObject(spProj);
			spProj->SetActive(false);
			m_lstProjectilePool.push_back(spProj);
		}
	}
	break;
	}
}

void Client::CProjectilePool::Release()
{
	if (m_spInitialDesc) { m_spInitialDesc = nullptr; }

	for (auto& spProj : m_lstProjectilePool) { spProj = nullptr; }
	m_lstProjectilePool.clear();

	for (auto& spProj : m_lstUsingPool) { spProj = nullptr; }
	m_lstUsingPool.clear();
}

void Client::CProjectilePool::Throw(Vector3 _vActivePos, Vector3 _vDirection, float _fPower, float _fDamage)
{
	if (!m_lstUsingPool.empty())
	{
		for (auto usingProjIter = m_lstUsingPool.begin(); usingProjIter != m_lstUsingPool.end();)
		{
			if (!(*usingProjIter)->IsActive())
			{
				m_lstProjectilePool.push_back(*usingProjIter);
				usingProjIter = m_lstUsingPool.erase(usingProjIter);
			}
			else
			{
				++usingProjIter;
			}
		}
	}

	if (m_lstProjectilePool.empty())
	{
		for (int32 i = 0; i < 3; ++i)
		{
			m_spInitialDesc->m_bDeferredAdd = false;
			shared_ptr<CProjectile> spProj = CProjectile::Create(m_spInitialDesc);
			m_wpLevel.lock()->AddGameObject(spProj);
			spProj->SetActive(false);
			m_lstProjectilePool.push_back(spProj);
		}
	}

	shared_ptr<CProjectile> spProj = m_lstProjectilePool.front();
	m_lstProjectilePool.pop_front();

	spProj->SetPosition(_vActivePos);
	spProj->SetActive(true);
	spProj->Throw(_vActivePos, _vDirection, _fPower, _fDamage);
	m_lstUsingPool.push_back(spProj);
}
