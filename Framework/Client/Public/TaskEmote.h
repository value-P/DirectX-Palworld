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
	struct FTaskEmoteDesc : public FBTNodeDesc
	{
		int32 m_iAnimationID = -1;

		float m_fMaxEmoteDeltaSeconds = 5.0f;
	};

	class CTaskEmote final : public CTaskNode
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CTaskEmote() = default;

		virtual ~CTaskEmote() = default;

	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CTaskEmote> Create(const std::shared_ptr<FTaskEmoteDesc>& _spTaskEmoteDesc);

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
		int32 m_iAnimationID = -1;

	private:
		float m_fMaxEmoteDeltaSeconds = 5.0f;

		float m_fSumEmoteDeltaSeconds = 0.0f;

		std::optional<Vector3> m_vPrePosition = nullopt;
	};
}
