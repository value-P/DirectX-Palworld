#pragma once
#include "TaskNode.h"

namespace Engine
{
	class CMeshComponent;
}

namespace Client
{
	class CMonster;

	/********************
		Description
	********************/
	struct FTaskReactDesc : public FBTNodeDesc
	{
		int32 m_iEatAnimationID = -1;

		int32 m_iPetAnimationID = -1;

		int32 m_iSlaughterAnimationID = -1;
	};

	class CTaskReact final : public CTaskNode
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CTaskReact() = default;

		virtual ~CTaskReact() = default;

	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CTaskReact> Create(const std::shared_ptr<FTaskReactDesc>& _spTaskReactDesc);

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
		std::shared_ptr<CMeshComponent> m_spMeshComponent = nullptr;

		std::shared_ptr<CMonster> m_spMonster = nullptr;

	private:
		int32 m_iEatAnimationID = -1;

		int32 m_iPetAnimationID = -1;

		int32 m_iSlaughterAnimationID = -1;

		EMonsterInteractType m_eMonsterInteractType = EMonsterInteractType::ENUM_END;

	private:
		const float m_fMaxReactDeltaSeconds = 4.0f;

		float m_fSumReactDeltaSeconds = 0.0f;

		std::optional<Vector3> m_vPrePosition = nullopt;
	};
}
