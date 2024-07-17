#pragma once
#include "HState.h"

namespace Client
{
	struct FHSElecPandaSkillDesc : public FHStateDesc
	{
		//EElecPandaSkillType m_eElecPandaSkillType = EElecPandaSkillType::ENUM_END;
	};

	class CHSElecPandaSkill : public CHState
	{
	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CHSElecPandaSkill> Create(const std::shared_ptr<FHSElecPandaSkillDesc> _spHSElecPandaSkillDesc);

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
		//EElecPandaSkillType m_eElecPandaSkillType = EElecPandaSkillType::ENUM_END;
	};
}
