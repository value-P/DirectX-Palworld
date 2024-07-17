#pragma once
#include "DecoratorNode.h"

namespace Client
{
	class CMonster;

	/********************
		Description
	********************/
	struct FDecoratorSkillTypeDesc : public FDecoratorNodeDesc
	{
		int32 m_iSkillType = -1;
	};

	class CDecoratorSkillType final : public CDecoratorNode
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CDecoratorSkillType() = default;

		virtual ~CDecoratorSkillType() = default;
		
	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CDecoratorSkillType> Create(const std::shared_ptr<FDecoratorSkillTypeDesc>& _spDecoratorHungerDesc);

		virtual HRESULT Initialize(const std::shared_ptr<FBTNodeDesc>& _spNodeDesc);

		virtual bool EvaluateDecoNode() override;
		
		virtual void Release() override;

	/********************
		Data Members
	********************/
	private:
		std::shared_ptr<CMonster> m_spMonster = nullptr;

		int32 m_iSkillType = -1;
	};
}
