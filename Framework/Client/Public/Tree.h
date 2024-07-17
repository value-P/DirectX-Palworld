#pragma once
#include "Nature.h"

namespace Engine
{
    class CBoxComponent;
    class CMeshComponent;
}

namespace Client
{
    struct FTreeDesc : public FNatureDesc
    {
		Vector3 m_vCenter = Vector3(0.0f);

		Vector3 m_vExtents = Vector3(1.0f);

		float m_fCurHp = float(INT_MAX);
    };

	class CTree final : public CNature
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CTree() = default;

		virtual ~CTree() = default;

	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CTree> Create(const std::shared_ptr<FTreeDesc>& _spTreeDesc);

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
		const Vector3& GetCenter() const { return m_vCenter; }

		const Vector3& GetExtents() const { return m_vExtents; }

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
		std::shared_ptr<CBoxComponent> m_spBodyBoxComponent = nullptr;

	private:
		Vector3 m_vCenter = Vector3(0.0f);

		Vector3 m_vExtents = Vector3(1.0f);

	private:
		float m_fSumCutDownDeltaSeconds = 0.0f;
    };
}
