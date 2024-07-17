#pragma once
#include "Level.h"

namespace Client
{
	class CGameManager;
	class CPlayer;

	class CTowerLevel final : public CLevel
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
		Data Members
	********************/
	private:
		std::weak_ptr<CGameManager> m_wpGameManager;

		std::shared_ptr<CPlayer> m_spPlayer = nullptr;
	};
}
