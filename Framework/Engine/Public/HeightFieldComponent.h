#pragma once
#include "ColliderComponent.h"

namespace Engine
{
	class CPhysXManager;
	class CModel;

	struct FHeightFieldComponentDesc : public FColliderComponentDesc
	{
		std::wstring m_wstrHeightFieldFilePath = L"";

		int32 m_iWidth = 1;

		int32 m_iHeight = 1;

		Vector3 m_vHeightFieldScale = Vector3(1.0f, 1.0f, 1.0f);

		Vector3 m_vHeightFieldRotate = Vector3(0.0f, 0.0f, 0.0f);

		Vector3 m_vHeightFieldTranslate = Vector3(0.0f, 0.0f, 0.0f);
	};

	class ENGINE_DLL CHeightFieldComponent final : public CColliderComponent
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CHeightFieldComponent() = default;

		virtual ~CHeightFieldComponent() = default;

	/********************
		Events
	********************/
	public:
		static shared_ptr<CHeightFieldComponent> Create(const std::shared_ptr<FComponentDesc>& _spComponentDesc);

		virtual HRESULT InitializeComponent(const std::shared_ptr<FComponentDesc>& _spComponentDesc) override;

	public:
		virtual HRESULT BeginPlay() override;

		virtual int32 PreTickComponent(float _fDeltaSeconds) override;

		virtual int32 TickComponent(float _fDeltaSeconds) override;

		virtual int32 PostTickComponent(float _fDeltaSeconds) override;

		virtual HRESULT EndPlay() override;

		virtual void Render(int32 _iShaderPath) override;

		virtual void Release() override;

	/********************
		Methods
	********************/
	protected:
		HRESULT CreateHeightFieldFromFile();

	/********************
		Data Members
	********************/
	protected:
		// PxRigidStatic* m_pStaticRigidActor = nullptr;

		std::shared_ptr<CModel> m_spModel = nullptr;

	protected:
		std::wstring m_wstrHeightFieldFilePath = L"";

		Vector3 m_vScale = Vector3::One;

		int32 m_iWidth = 1;

		int32 m_iHeight = 1;

		Vector3 m_vHeightFieldScale = Vector3::One;

		Vector3 m_vHeightFieldRotate = Vector3::Zero;

		Vector3 m_vHeightFieldTranslate = Vector3::Zero;
	};
}
