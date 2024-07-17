#pragma once
#include "Monster.h"

namespace Client
{
	/********************
		Descriptor
	********************/
	struct FXenogrifDesc : public FMonsterDesc
	{
	};

	class CXenogrif : public CMonster
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CXenogrif() = default;

		virtual ~CXenogrif() = default;
		
	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CXenogrif> Create(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc);

		virtual HRESULT Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc) override;

	public:
		virtual HRESULT BeginPlay() override;

		virtual int32 PreTick(float _fDeltaSeconds) override;

		virtual int32 Tick(float _fDeltaSeconds) override;
		
		virtual int32 PostTick(float _fDeltaSeconds) override;

		virtual HRESULT EndPlay() override;

		virtual void Release() override;
	};
}
