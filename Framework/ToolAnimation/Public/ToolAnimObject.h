#pragma once
#include "GameObject.h"

namespace Engine
{
	class CMeshComponent;
}

namespace ToolAnimation
{
	struct FToolAnimObjectDesc : public CGameObject::FGameObjectDesc
	{
		wstring m_wstrAnimModelPath = L"";
		Matrix m_matPivot;
	};

    class CToolAnimObject : public CGameObject
    {
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CToolAnimObject() = default;

		virtual ~CToolAnimObject() = default;

	/********************
		Getter/Setter
	********************/
	public:
		shared_ptr<CMeshComponent> GetMeshComponent() { return m_spMeshComponent; }

	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CToolAnimObject> Create(const std::shared_ptr<FToolAnimObjectDesc>& _spPlayerDesc);

		virtual HRESULT Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc) override;

		virtual HRESULT BeginPlay() override;

		virtual int32 PreTick(float _fDeltaSeconds) override;

		virtual int32 Tick(float _fDeltaSeconds) override;

		virtual int32 PostTick(float _fDeltaSeconds) override;

		virtual HRESULT EndPlay() override;

		virtual void Release() override;

	private:
		void InitMeshComponent(const std::shared_ptr<FToolAnimObjectDesc>& _spPlayerDesc);

	/********************
		Methods
	********************/
	private:

	/********************
		Data Members
	********************/
	private:
		shared_ptr<CMeshComponent> m_spMeshComponent = nullptr;
    };
}


