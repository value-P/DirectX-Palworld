#pragma once
#include "DecoratorNode.h"

namespace Client
{
	class CMonster;

	struct FDecoratorPartnerSkillDesc : public FDecoratorNodeDesc
	{
	};

    class CDecoratorPartnerSkill : public CDecoratorNode
    {
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CDecoratorPartnerSkill() = default;

		virtual ~CDecoratorPartnerSkill() = default;

	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CDecoratorPartnerSkill> Create(const std::shared_ptr<FDecoratorPartnerSkillDesc>& _spDecoratorPartnerSkillDesc);

		virtual HRESULT Initialize(const std::shared_ptr<FBTNodeDesc>& _spNodeDesc);

		virtual bool EvaluateDecoNode() override;
		
		virtual void Release() override;

	/********************
		Data Members
	********************/
	private:
		std::shared_ptr<CMonster> m_spMonster = nullptr;

		bool m_bCurPartnerSkill = false;
    };
}
