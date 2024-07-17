#pragma once
#include "Npc.h"

namespace Client
{
	class CWeapon;

    class CNpc_Quest05 final : public CNpc
    {
    /********************
		Ctor/Dtor
	********************/
	public:
		explicit CNpc_Quest05() = default;

		virtual ~CNpc_Quest05() = default;

	/********************
		Events
	********************/
	public:
		static shared_ptr<CNpc_Quest05> Create(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc);

		virtual HRESULT Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc) override;

		virtual HRESULT BeginPlay() override;

		virtual int32 PreTick(float _fDeltaSeconds) override;

		virtual int32 Tick(float _fDeltaSeconds) override;

		virtual int32 PostTick(float _fDeltaSeconds) override;

		virtual HRESULT EndPlay() override;

		virtual void Release() override;

	protected:
		virtual void InitCycle() override;

	/********************
		Getter/Setter
	********************/
	public:

	/********************
		Methods
	********************/
	public:		

	/********************
		Data Members
	********************/
	protected:
		shared_ptr<CWeapon> m_spAxe = nullptr;
    };
}