#pragma once
#include "Level.h"

namespace Client
{
	class CPlayer;

    class CPersistentLevel final : public CLevel
    {
	/********************
		Events
	********************/
	public:
		virtual HRESULT Initialize() override;

		virtual HRESULT BeginPlay() override;
		
		virtual int32 PreTick(float _fDeltaSeconds) override;

		virtual int32 Tick(float _fDeltaSeconds) override;

		virtual int32 PostTick(float _fDeltaSeconds) override;

		virtual void Release() override;
		
    /********************
		Methods
	********************/

    };
}
