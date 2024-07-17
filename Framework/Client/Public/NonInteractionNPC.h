#pragma once
#include "Npc.h"

namespace Client
{
	struct FNonInteractionNPC : public FNpcDesc
	{
		ENPCANIM m_eDefaultAnim = ENPCANIM::AS_Player_Female_Idle_None;
	};

    class CNonInteractionNPC final : public CNpc
    {
    /********************
		Ctor/Dtor
	********************/
	public:
		explicit CNonInteractionNPC() = default;

		virtual ~CNonInteractionNPC() = default;

	/********************
		Events
	********************/
	public:
		static shared_ptr<CNonInteractionNPC> Create(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc);

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
		Data Members
	********************/
	public:
		virtual void Interact() override;

	/********************
		Data Members
	********************/
	protected:
    };
}