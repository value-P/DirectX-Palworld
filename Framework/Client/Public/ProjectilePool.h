#pragma once
#include "Projectile.h"

namespace Engine
{
	class CLevel;
}

namespace Client
{
	class CProjectile;

	enum class EProjectileType
	{
		DEFAULT,
		PALSPHERE,
		PARTNER_PALSPHERE,
		RIDING_PALSPHERE,
		ARROW,
		ENUM_END,
	};

	class CProjectilePool final
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CProjectilePool() = default;

		virtual ~CProjectilePool() = default;

	/********************
		Events
	********************/
	public:
		static shared_ptr<CProjectilePool> Create(const shared_ptr<CLevel> _spLevel, EProjectileType _eProjType, const shared_ptr<FProjectileDesc>& _spInitDesc, int32 _iSize);

		void Initialize(const shared_ptr<CLevel> _spLevel, EProjectileType _eProjType, const shared_ptr<FProjectileDesc>& _spInitDesc, int32 _iSize);
		
		void Release();

	/********************
		Methods
	********************/
	public:
		void Throw(Vector3 _vActivePos, Vector3 _vDirection, float _fPower, float _fDamage);

	/********************
		Data Members
	********************/
	private:
		shared_ptr<FProjectileDesc> m_spInitialDesc = nullptr;
		int32 m_iCount = 0;

		weak_ptr<CLevel> m_wpLevel;
		list<shared_ptr<CProjectile>> m_lstProjectilePool;
		list<shared_ptr<CProjectile>> m_lstUsingPool;
	};
}