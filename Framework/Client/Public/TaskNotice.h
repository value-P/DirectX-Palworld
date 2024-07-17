#pragma once
#include "TaskNode.h"

namespace Engine
{
	class CMeshComponent;
}

namespace Client
{
	class CEffectManager;
	class CMonster;

	struct FTaskNoticeDesc : public FBTNodeDesc
	{
		EMonsterNoticeType m_eMonsterNoticeType = EMonsterNoticeType::ENUM_END;

		int32 m_iAnimationID = -1;
	};

	class CTaskNotice final : public CTaskNode
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CTaskNotice() = default;

		virtual ~CTaskNotice() = default;

	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CTaskNotice> Create(const std::shared_ptr<FTaskNoticeDesc>& _spTaskNoticeDesc);

		virtual HRESULT Initialize(const shared_ptr<FBTNodeDesc>& _spNodeDesc) override;

	public:
		virtual void OnNodeInit(float _fDeltaSecond) override;

		virtual void OnNodeEnter(float _fDeltaSecond) override;

		virtual EBTNodeResult ExecuteTaskNode(float _fDeltaSecond) override;

		virtual void Release() override;

	/********************
		Data Members
	********************/
	private:
		weak_ptr<CEffectManager> m_wpEffectManager;

	private:
		std::shared_ptr<CMeshComponent> m_spMeshComponent = nullptr;

		std::shared_ptr<CMonster> m_spMonster = nullptr;

	private:
		int32 m_iAnimationID = -1;

		EMonsterNoticeType m_eMonsterNoticeType = EMonsterNoticeType::ENUM_END;

	private:
		const float m_fMaxNoticeDeltaSeconds = 2.0f;

		float m_fSumNoticeDeltaSeconds = 0.0f;

		std::optional<Vector3> m_vPrePosition = nullopt;
	};
}
