#pragma once
#include "GameObject.h"

namespace Client
{
	class CEffectGroup;

	struct FEffectBaseDesc : public CGameObject::FGameObjectDesc
	{
		EEffectType m_eEffectType = EEffectType::ENUM_END;
	};

	class CEffectBase abstract : public CGameObject
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CEffectBase() = default;
		virtual ~CEffectBase() = default;
		
	/********************
		Events
	********************/
	public:
		virtual HRESULT Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc);

		virtual HRESULT BeginPlay() override;

		virtual int32 PreTick(float _fDeltaSeconds) override;

		virtual int32 Tick(float _fDeltaSeconds) override;

		virtual int32 PostTick(float _fDeltaSeconds) override;

		virtual HRESULT EndPlay() override;

		virtual void Release() override;
		
	/********************
		Methods
	********************/
	public:
		

	/********************
		Getter / Setter
	********************/
	public:
		EEffectType GetEffectType() { return m_eEffectType; }

		void SetOwnerEffectGroup(std::shared_ptr<CEffectGroup> _spEffectGroup) { m_wpEffectGroup = _spEffectGroup; }

		const Matrix& GetOriginMatrix() { return m_matOrigin; }

	/********************
		Data Members
	********************/
	protected:
		Matrix m_matOrigin = XMMatrixIdentity();

		EEffectType m_eEffectType = EEffectType::ENUM_END;

		std::weak_ptr<CEffectGroup> m_wpEffectGroup;
	};
}
