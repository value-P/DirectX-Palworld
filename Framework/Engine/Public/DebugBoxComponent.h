#pragma once
#include "DebugColliderComponent.h"

namespace Engine
{
	/********************
		Description
	********************/
	struct ENGINE_DLL FDebugBoxComponentDesc : public FDebugColliderComponentDesc
	{
		Vector3 m_vExtents = Vector3(1.0f, 1.0f, 1.0f);
	};

	class ENGINE_DLL CDebugBoxComponent : public CDebugColliderComponent
	{
	/********************
			Ctor/Dtor
		********************/
	public:
		explicit CDebugBoxComponent() = default;

		virtual ~CDebugBoxComponent() = default;

	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CDebugBoxComponent> Create(const std::shared_ptr<FDebugBoxComponentDesc>& _spDebugBoxComponentDesc);

		virtual HRESULT InitializeComponent(const std::shared_ptr<FComponentDesc>& _spComponentDesc) override;

	public:
		virtual HRESULT BeginPlay() override;

		virtual int32 PreTickComponent(float _fDeltaSeconds) override;

		virtual int32 TickComponent(float _fDeltaSeconds) override;

		virtual int32 PostTickComponent(float _fDeltaSeconds) override;

		virtual HRESULT EndPlay() override;

		virtual void Render(int32 _iShaderPath) override;

		virtual void Release() override;

		virtual HRESULT BindVariable() override;


	/********************
		Getter/Setter
	********************/
	public:
		void SetExtents(const Vector3& _vExtents) { m_vExtents = _vExtents; }

		const Vector3& GetExtents() const { return m_vExtents; }

	/********************
		Data Members
	********************/
	private:
		Vector3 m_vExtents = Vector3::One;

	private:
		// DX OBB ¹Ú½º
		BoundingBox m_tDxLocalOBB{};

		BoundingBox m_tDxWorldOBB{};
	};
}

