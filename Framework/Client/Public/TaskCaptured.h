#pragma once
#include "TaskNode.h"

namespace Engine
{
	class CMeshComponent;
}

namespace Client
{
	class CEffectManager;
	class CCreature;
	class CMonster;

	struct FTaskCapturedDesc : public FBTNodeDesc
	{
	};

	class CTaskCaptured final : public CTaskNode
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CTaskCaptured() = default;

		virtual ~CTaskCaptured() = default;

	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CTaskCaptured> Create(const std::shared_ptr<FTaskCapturedDesc>& _spTaskCapturedDesc);

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
		std::weak_ptr<CMonster> m_wpMonster;

		bool m_bCaptured = false;

	private:
		std::weak_ptr<CEffectManager> m_wpEffectManager;
	};
}
