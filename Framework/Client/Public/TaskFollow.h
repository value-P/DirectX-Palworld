#pragma once

#include "TaskNode.h"

namespace Engine
{
	class CMeshComponent;
	class CCameraComponent;
}

namespace Client
{
	class CMonster;
	class CPlayer;

	/********************
		Description
	********************/
	struct FTaskFollowDesc : public FBTNodeDesc
	{
		std::shared_ptr<CPlayer> m_spPlayer = nullptr;

		int32 m_iWalkAnimationID = -1;

		int32 m_iRunAnimationID = -1;
	};

	class CTaskFollow final : public CTaskNode
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CTaskFollow() = default;

		virtual ~CTaskFollow() = default;

	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CTaskFollow> Create(const std::shared_ptr<FTaskFollowDesc>& _spTaskFollowDesc);

		virtual HRESULT Initialize(const shared_ptr<FBTNodeDesc>& _spNodeDesc) override;

	public:
		virtual void OnNodeEnter(float _fDeltaSecond) override;

		virtual EBTNodeResult ExecuteTaskNode(float _fDeltaSecond) override;

		virtual void OnNodeInit(float _fDeltaSecond) override;

		virtual void Release() override;
		
	/********************
		Methods
	********************/
	public:
		void UpdatePalPosition();

	/********************
		Data Members
	********************/
	private:
		std::shared_ptr<CMeshComponent> m_spMeshComponent = nullptr;
		
		std::shared_ptr<CCameraComponent> m_spCameraComponent = nullptr;

		std::shared_ptr<CMonster> m_spMonster = nullptr;

		std::weak_ptr<CPlayer> m_wpPlayer;

	private:
		int32 m_iWalkAnimationID = -1;

		int32 m_iRunAnimationID = -1;

	private:
		float m_fDistanceRatio = 5.0f;

		float m_fMoveSpeedRatio = 1.0f;

		float m_fSumLerpDeltaSeconds = 0.0f;

		const float m_fMaxLerpDeltaSeconds = 2.0f;

		const float m_fNearDistance = 1.5f;

		const float m_fFarDistance = 3.0f;
	};
}
