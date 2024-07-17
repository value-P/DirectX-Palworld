#pragma once
#include "Nature.h"

namespace Engine
{
	class CSphereComponent;
	class CMeshComponent;
}

namespace Client
{
	struct FStoneDesc : public FNatureDesc
	{
		Vector3 m_vCenter = Vector3(0.0f, 0.0f, 0.0f);

		float m_fRadius = 1.0f;
	};

    class CStone final : public CNature
    {
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CStone() = default;

		virtual ~CStone() = default;
		
	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CStone> Create(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc);

		virtual HRESULT Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc) override;

	public:
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
		virtual HRESULT AddHabitatToScene() override;

		virtual HRESULT RemoveHabitatFromScene() override;

	/********************
		Getter/Setter
	********************/
	public:
		float GetRadius() const { return m_fRadius; }

		const Vector3& GetCenter() const { return m_vCenter; }

	/********************
		Data Members
	********************/
	private:
		std::shared_ptr<CSphereComponent> m_spSphereComponent = nullptr;

	private:
		Vector3 m_vCenter = Vector3(0.0f, 0.0f, 0.0f);

		float m_fRadius = 1.0f;
    };
}
