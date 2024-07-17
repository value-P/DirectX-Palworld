#pragma once
#include "Projectile.h"

namespace Client
{
	class CMonster;

    class CArrow : public CProjectile
    {
    /********************
		Ctor/Dtor
	********************/
	public:
		explicit CArrow() = default;

		virtual ~CArrow() = default;
		
	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CArrow> Create(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc);

		virtual HRESULT Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc) override;

		virtual HRESULT BeginPlay() override;

		virtual int32 PreTick(float _fDeltaSeconds) override;

		virtual int32 Tick(float _fDeltaSeconds) override;
		
		virtual int32 PostTick(float _fDeltaSeconds) override;

		virtual HRESULT EndPlay() override;

		virtual void Release() override;
		
	/********************
		Overrides
	********************/
	public:
		virtual void OnTriggerEnter(CComponent* _pSrcComponent, CComponent* _pDstComponent) override;
		
	private:
		virtual void InitTrailComponent() override;

	/********************
		Methods
	********************/
	protected:
		virtual void ResetDataMember() override;

		virtual void Throw(Vector3 _vActivePos, Vector3 _vDirection, float _fPower, float _fDamage);

	/********************
		Data Members
	********************/
	private:
		
    };
}