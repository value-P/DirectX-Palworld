#pragma once
#include "Projectile.h"

namespace Client
{
	class CPalRetrieveTrail;

	class CRidingPalSphere final : public CProjectile
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CRidingPalSphere() = default;

		virtual ~CRidingPalSphere() = default;
		
	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CRidingPalSphere> Create(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc);

		virtual HRESULT Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc) override;

		virtual HRESULT BeginPlay() override;

		virtual int32 PreTick(float _fDeltaSeconds) override;

		virtual int32 Tick(float _fDeltaSeconds) override;
		
		virtual int32 PostTick(float _fDeltaSeconds) override;

		virtual HRESULT EndPlay() override;

		virtual void Release() override;
		
	public:
		virtual void InitTrailComponent() override;

	/********************
		Overrides
	********************/
	public:
		virtual void OnTriggerEnter(CComponent* _pSrcComponent, CComponent* _pDstComponent) override;
		

	/********************
		Methods
	********************/
	protected:
		virtual void ResetDataMember() override;

	/********************
		Data Members
	********************/
	private:

	};
}