#pragma once
#include "HState.h"

namespace Engine
{
	class CGameObject;
}

namespace Client
{
	class CMonster;

	struct FHSElecPandaMoveDesc : public FHStateDesc
	{
		EElecPandaMoveType m_eElecPandaMoveType = EElecPandaMoveType::ENUM_END;

		std::shared_ptr<CGameObject> m_spTarget = nullptr;

		float m_fDistanceRange = 1.0f;
	};

	class CHSElecPandaMove final : public CHState
	{
	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CHSElecPandaMove> Create(const std::shared_ptr<FHSElecPandaMoveDesc> _spHSElecPandaMoveDesc);

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

	public:
		void SetDstPosition(const Vector3& _vDstPosition) { m_vDstPosition = _vDstPosition; }

		void SetDstDirection(const Vector3& _vDstDirection) { m_vDstDirection = _vDstDirection; }

	/********************
		Data Members
	********************/
	private:
		EElecPandaMoveType m_eElecPandaMoveType = EElecPandaMoveType::ENUM_END;

		std::shared_ptr<CMonster> m_spMonster = nullptr;

		std::shared_ptr<CGameObject> m_spTarget = nullptr;

	private:
		Vector3 m_vDstPosition = Vector3(0.0f);

		Vector3 m_vDstDirection = Vector3(0.0f);

		float m_fDistanceRange = 1.0f;

		float m_fMaxLerpDeltaSeconds = 2.0f;

		float m_fSumLerpDeltaSeconds = 0.0f;
	};
}
