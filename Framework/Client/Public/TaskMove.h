#pragma once
#include "TaskNode.h"

namespace Engine
{
	class CMeshComponent;
}

namespace Client
{
	class CMonster;
	class CPlayer;

	/********************
		Description
	********************/
	struct FTaskMoveDesc : public FBTNodeDesc
	{
		int32 m_iAnimationID = -1;

		Vector3 m_vSpawnPosition = Vector3(0.0f);
	};

	class CTaskMove : public CTaskNode
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CTaskMove() = default;

		virtual ~CTaskMove() = default;

	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CTaskMove> Create(const std::shared_ptr<FTaskMoveDesc>& _spTaskMoveDesc);

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
		int32 m_iAnimationID = -1;

		std::shared_ptr<CMeshComponent> m_spMeshComponent = nullptr;
		
		std::shared_ptr<CMonster> m_spMonster = nullptr;

		std::weak_ptr<CGameObject> m_wpTarget;

		EMonsterType m_eMonsterType = EMonsterType::ENUM_END;

	private:
		Vector3 m_vDstPosition = Vector3(0.0f);

		float m_fAcceptableRadius = 1.0f;

		float m_fSumLerpDeltaSeconds = 0.0f;

		float m_fMaxLerpDeltaSeconds = 1.0f;

	private:
		Vector3 m_vSpawnPosition = Vector3(0.0f);

		bool m_bCreep = false;
	};
}
