#pragma once
#include "GameObject.h"

namespace Client
{
	struct FTowerDesc : public CGameObject::FGameObjectDesc
	{

	};

	class CTower final : public CGameObject
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CTower() = default;

		virtual ~CTower() = default;

	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CTower> Create(const std::shared_ptr<FGameObjectDesc>& _spBossRoomDesc);

		virtual HRESULT Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc);

	public:
		virtual HRESULT BeginPlay() override;

		virtual int32 PreTick(float _fDeltaSeconds) override;

		virtual int32 Tick(float _fDeltaSeconds) override;

		virtual int32 PostTick(float _fDeltaSeconds) override;

		virtual HRESULT EndPlay() override;

		virtual void Release() override;

	/********************
		Data Members
	********************/
	private:

	};
}