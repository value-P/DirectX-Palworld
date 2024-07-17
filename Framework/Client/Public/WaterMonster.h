#pragma once
#include "Monster.h"

namespace Client
{
	class CWaterMonster : public CMonster
	{
	/********************
		Descriptor
	********************/
	public:
		struct FWaterMonsterDesc : FMonsterDesc
		{
		};

	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CWaterMonster() = default;

		virtual ~CWaterMonster() = default;
		
	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CWaterMonster> Create(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc);

		virtual HRESULT Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc);

		virtual HRESULT BeginPlay();

		virtual int32 PreTick(float _fDeltaSeconds);

		virtual int32 Tick(float _fDeltaSeconds);
		
		virtual int32 PostTick(float _fDeltaSeconds);

		virtual HRESULT EndPlay();

		virtual void Release();
		
	/********************
		Getter/Setter
	********************/

	/********************
		Data Members
	********************/
	};
}


