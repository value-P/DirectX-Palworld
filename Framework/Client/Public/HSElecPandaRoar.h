#pragma once
#include "HState.h"

namespace Client
{
	class CMonster;

	struct FHSElecPandaRoarDesc : public FHStateDesc
	{
	};

	class CHSElecPandaRoar final : public CHState
	{
	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CHSElecPandaRoar> Create(const std::shared_ptr<FHSElecPandaRoarDesc> _spHSElecPandaRoarDesc);

		virtual HRESULT Initialize(const std::shared_ptr<FHStateDesc>& _spHStateDesc) override;

	public:
		virtual int32 Tick(float _fDeltaSeconds);

		virtual void Release();

	/********************
		Methods
	********************/
	public:
		virtual HRESULT ResetHState() override;

		virtual HRESULT ReserveCommand
		(
			const std::vector<std::tuple<int32, int32>>& _vecCommand,
			EHFSMReserveCommandType _eHFSMReserveCommandType
		) override;

		virtual HRESULT EvaluateHState() override;

		virtual HRESULT ReconstructHState() override;
		
	/********************
		Data Members
	********************/
	private:
		std::shared_ptr<CMonster> m_spMonster = nullptr;
	};
}
