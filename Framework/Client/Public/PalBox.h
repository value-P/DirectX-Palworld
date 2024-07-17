#pragma once
#include "Architecture.h"

namespace Engine
{
	class CSphereComponent;
	class CPanel;
}

namespace Client
{
	class CEffectGroup;

	struct FPalBoxDesc: public FArchitectureDesc
	{
	};

	class CPalBox final : public CArchitecture
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CPalBox() = default;

		virtual ~CPalBox() = default;

	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CPalBox> Create(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc);

		virtual HRESULT Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc) override;

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
		virtual void ClearEffect() override;

	private:
		virtual HRESULT InitializeMeshComponent() override;

		virtual HRESULT InitializeColliderComponent() override;

	/********************
		Getter/Setter
	********************/
	public:
		float GetRadius() { return m_fRangeRadius; }

	/********************
		Data Members
	********************/
	protected:
		std::shared_ptr<CSphereComponent> m_spBaseCampSphereComponent = nullptr;

		std::weak_ptr<CEffectGroup> m_wpRangeEffect;

		const float m_fRangeRadius = 20.0f;
	};
}
