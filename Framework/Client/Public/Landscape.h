#pragma once
#include "GameObject.h"

namespace Engine
{
	class CMeshComponent;
	class CHeightFieldComponent;
}

namespace Client
{
	struct FLandscapeDesc : public CGameObject::FGameObjectDesc
	{
		std::wstring m_wstrLandscapeFilePath = L"";

		int32 m_iWidth = 1;

		int32 m_iHeight = 1;

		Vector3 m_vLandscapeScale = Vector3(1.0f, 1.0f, 1.0f);

		Vector3 m_vLandscapeRotate = Vector3(0.0f, 0.0f, 0.0f);

		Vector3 m_vLandscapeTranslate = Vector3(0.0f, 0.0f, 0.0f);
	};

	class CLandscape final : public CGameObject
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CLandscape() = default;

		virtual ~CLandscape() = default;

	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CLandscape> Create(const std::shared_ptr<FLandscapeDesc>& _spLandscapeDesc);

		virtual HRESULT Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc);

	public:
		virtual HRESULT BeginPlay() override;

		virtual int32 PreTick(float _fDeltaSeconds) override;

		virtual int32 Tick(float _fDeltaSeconds) override;

		virtual int32 PostTick(float _fDeltaSeconds) override;

		virtual HRESULT EndPlay() override;

		virtual void Release() override;

	/********************
		Data Members
	********************/
	private:
		std::shared_ptr<CMeshComponent> m_spMeshComponent = nullptr;

		std::shared_ptr<CHeightFieldComponent> m_spHeightFieldComponent = nullptr;
	};
}
