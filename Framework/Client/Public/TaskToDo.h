#pragma once
#include "TaskNode.h"

namespace Engine
{
	class CMeshComponent;
	class CBone;
}

namespace Client
{
	class CEffectManager;
	class CEffectGroup;
	class CMonster;

	/********************
		Description
	********************/
	struct FTaskToDoDesc : public FBTNodeDesc
	{
		// »ý»ê
		int32 m_iProduceAnimationID = -1;

		// °Ç¼³
		int32 m_iBuildAnimationID = -1;

		// Ã¤±¼
		int32 m_iMineAnimationID = -1;

		// ¹ú¸ñ
		int32 m_iCutDownAnimationID = -1;

		// ¿î¹Ý
		int32 m_iTransportAnimationID = -1;

		// ½Ä¹°(¾¾¾Ñ ½É±â, ¹° ÁÖ±â)
		int32 m_iPlantAnimationID = -1;

		// ºÒ
		int32 m_iCookStartAnimationID = -1;

		int32 m_iCookLoopAnimationID = -1;
	};

	class CTaskToDo final : public CTaskNode
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CTaskToDo() = default;

		virtual ~CTaskToDo() = default;

	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CTaskToDo> Create(const std::shared_ptr<FTaskToDoDesc>& _spTaskToDoDesc);

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

		std::shared_ptr<CMeshComponent> m_spMeshComponent = nullptr;

	private:
		// Á¦ÀÛ
		int32 m_iProduceAnimationID = -1;

		// °Ç¼³
		int32 m_iBuildAnimationID = -1;

		// Ã¤±¼
		int32 m_iMineAnimationID = -1;

		// ¹ú¸ñ
		int32 m_iCutDownAnimationID = -1;

		// ¿î¹Ý
		int32 m_iTransportAnimationID = -1;

		// ½Ä¹°(¾¾¾Ñ ½É±â, ¹° ÁÖ±â)
		int32 m_iPlantAnimationID = -1;

		// ºÒ
		int32 m_iCookStartAnimationID = -1;

		int32 m_iCookLoopAnimationID = -1;

		EToDoType m_eToDoType = EToDoType::ENUM_END;

	private:
		std::optional<Vector3> m_vPrePosition = nullopt;

	private:
		std::weak_ptr<CEffectManager> m_wpEffectManager;

		weak_ptr<CBone> m_wpMouthBone;

		weak_ptr<CEffectGroup> m_wpFireBreath;
	};
}
