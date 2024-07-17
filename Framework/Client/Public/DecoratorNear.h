#pragma once
#include "DecoratorNode.h"

namespace Client
{
	class CMonster;
	class CPlayer;

	class FDecoratorNearDesc : public FDecoratorNodeDesc
	{
	public:
		std::shared_ptr<CPlayer> m_spPlayer = nullptr;

		float m_fRadius = 5.0f;
	};

	class CDecoratorNear : public CDecoratorNode
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CDecoratorNear() = default;

		virtual ~CDecoratorNear() = default;

	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CDecoratorNear> Create(const std::shared_ptr<FDecoratorNearDesc>& _spDecoratorNearDesc);

		virtual HRESULT Initialize(const std::shared_ptr<FBTNodeDesc>& _spNodeDesc);

		virtual bool EvaluateDecoNode() override;
		
		virtual void Release() override;

	/********************
		Data Members
	********************/
	private:
		std::shared_ptr<CMonster> m_spMonster = nullptr;

		std::shared_ptr<CPlayer> m_spPlayer = nullptr;

		float m_fRadius = 5.0f;
	};
}
