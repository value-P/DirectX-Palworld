#pragma once
#include "Npc.h"

namespace Client
{
    class CNpc_Merchant : public CNpc
    {
    /********************
		Ctor/Dtor
	********************/
	public:
		explicit CNpc_Merchant() = default;

		virtual ~CNpc_Merchant() = default;

	/********************
		Events
	********************/
	public:
		static shared_ptr<CNpc_Merchant> Create(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc);

		virtual HRESULT Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc) override;

		virtual HRESULT BeginPlay() override;

		virtual int32 PreTick(float _fDeltaSeconds) override;

		virtual int32 Tick(float _fDeltaSeconds) override;

		virtual int32 PostTick(float _fDeltaSeconds) override;

		virtual HRESULT EndPlay() override;

		virtual void Release() override;

	/********************
		Methods
	********************/
	public:
		virtual void Interact() override;

	protected:
		virtual void StateIdle(float _fDeltaSeconds) override;
		virtual void StateTalking(float _fDeltaSeconds) override;
		virtual void StateGive(float _fDeltaSeconds) override;
		virtual void StateUseDevice(float _fDeltaSeconds) override;

	/********************
		Data Members
	********************/
	protected:

    };
}


