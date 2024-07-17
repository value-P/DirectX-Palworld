#pragma once
#include "TaskNode.h"

namespace Engine
{
	class CGameObject;
	class CMeshComponent;
}

namespace Client
{
	class CCreature;
	class CMonster;
	
	/********************
		Description
	********************/
	struct FTaskEncountDesc : public FBTNodeDesc
	{
		std::shared_ptr<CCreature> m_spTarget = nullptr;

		int32 m_iAnimationID = -1;
	};

	class CTaskEncount : public CTaskNode
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CTaskEncount() = default;

		virtual ~CTaskEncount() = default;

	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CTaskEncount> Create(const std::shared_ptr<FTaskEncountDesc>& _spTaskEncountDesc);

		virtual HRESULT Initialize(const shared_ptr<FBTNodeDesc>& _spNodeDesc) override;

	public:
		virtual void OnNodeEnter(float _fDeltaSecond) override;

		virtual EBTNodeResult ExecuteTaskNode(float _fDeltaSecond) override;

		virtual void OnNodeInit(float _fDeltaSecond) override;

		virtual void Release() override;

	/********************
		Data Members
	********************/
	private:
		std::shared_ptr<CMonster> m_spMonster = nullptr;

		std::weak_ptr<CCreature> m_wpTarget;

		std::shared_ptr<CMeshComponent> m_spMeshComponent = nullptr;

	private:
		int32 m_iAnimationID = -1;

	private:
		float m_fSumLerpDeltaSeconds = 0.0f;

		const float m_fMaxLerpDeltaSeconds = 2.0f;

		std::optional<Vector3> m_vForward = std::nullopt;
	};
}