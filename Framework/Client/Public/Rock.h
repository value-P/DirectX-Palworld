#pragma once
#include "Nature.h"

namespace Engine
{
	class CSphereComponent;
	class CMeshComponent;
}

namespace Client
{
	struct FRockDesc : public FNatureDesc
	{
		Vector3 m_vCenter = Vector3(0.0f);

		float m_fRadius = 1.0f;

		float m_fCurHP = float(INT_MAX);
	};

	class CRock final : public CNature
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CRock() = default;

		virtual ~CRock() = default;
		
	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CRock> Create(const std::shared_ptr<FRockDesc>& _spRockDesc);

		virtual HRESULT Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc) override;

	public:
		virtual HRESULT BeginPlay() override;

		virtual int32 PreTick(float _fDeltaSeconds) override;

		virtual int32 Tick(float _fDeltaSeconds) override;
		
		virtual int32 PostTick(float _fDeltaSeconds) override;

		virtual HRESULT EndPlay() override;

		virtual void Release() override;
		
	/********************
		Getter/Setter
	********************/
	public:
		float GetRadius() const { return m_fRadius; }

		const Vector3& GetCenter() const { return m_vCenter; }

	/********************
		Methods
	********************/
	public:
		virtual HRESULT AddHabitatToScene() override;

		virtual HRESULT RemoveHabitatFromScene() override;

		virtual void TakeDamage(const std::shared_ptr<CGameObject>& _spGameObject, float _fDamage) override;

		virtual void RespawnNature(float _fDeltaSeconds) override;

		virtual bool DestroyNature(float _fDeltaSeconds) override;

		virtual float GetAcceptableRadius() override;

	public:
		virtual void OnTriggerEnter(CComponent* _pSrcComponent, CComponent* _pDstComponent) override;

	/********************
		Data Members
	********************/
	private:
		std::shared_ptr<CSphereComponent> m_spBodySphereComponent = nullptr;

	private:
		Vector3 m_vCenter = Vector3(0.0f);

		float m_fRadius = 1.0f;

	private:
		float m_fSumMineDeltaSeconds = 0.0f;
	};
}
