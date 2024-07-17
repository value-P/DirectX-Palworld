#pragma once
#include "ColliderComponent.h"

namespace Engine
{
	class CPhysXManager;
	class CModel;

	struct ENGINE_DLL FTriangleMeshComponentDesc : public FColliderComponentDesc
	{
	/********************
		Description
	********************/
	public:
		std::wstring m_wstrTriangleMeshFilePath = L"";

		bool m_bInsertDirectly = true;
	};

	class ENGINE_DLL CTriangleMeshComponent : public CColliderComponent
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CTriangleMeshComponent() = default;

		virtual ~CTriangleMeshComponent() = default;

	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CTriangleMeshComponent> Create(const std::shared_ptr<FTriangleMeshComponentDesc>& _spTriangleMeshComponentDesc);

		virtual HRESULT InitializeComponent(const std::shared_ptr<FComponentDesc>& _spComponentDesc) override;

	public:
		virtual HRESULT BeginPlay() override;

		virtual int32 PreTickComponent(float _fDeltaSeconds) override;

		virtual int32 TickComponent(float _fDeltaSeconds) override;

		virtual int32 PostTickComponent(float _fDeltaSeconds) override;

		virtual HRESULT EndPlay() override;

		virtual HRESULT BindVariable() override;

		virtual void Render(int32 _iShaderPath) override;

		virtual void Release() override;
		
	/********************
		Methods
	********************/
	private:
		void CreateTriangleMeshFromFile();

	public:
		HRESULT AddTriangleMeshToScene();

		HRESULT RemoveTriangleMeshFromScene();

		void SetSimulation(bool _bSimulation);

	/********************
		Data Members
	********************/
	protected:
		int32 m_iNumTriangleMeshes = 0;

		uint32 m_iTriangleMeshSize = 0;

		int32 m_iOutBufferSize = 0;

		std::vector<PxTriangleMesh*> m_vecTriangleMeshes;
		
		std::vector<PxRigidStatic*> m_vecRigidStatics;

		std::vector<shared_ptr<PxDefaultMemoryOutputStream>> m_vecOutBuffers;

		std::shared_ptr<CModel> m_spModel = nullptr;

	protected:
		std::wstring m_wstrTriangleMeshFilePath = L"";

		bool m_bInsertDirectly = true;
	};
}
