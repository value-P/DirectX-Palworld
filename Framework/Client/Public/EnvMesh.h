#pragma once
#include "GameObject.h"

namespace Engine
{
	class CMeshComponent;
}

namespace Client
{
	struct FEnvMeshDesc : public CGameObject::FGameObjectDesc
	{
		std::wstring m_wstrEnvMeshFilePath = L"";
	};

	class CEnvMesh final : public CGameObject
	{
	public:
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CEnvMesh() = default;

		virtual ~CEnvMesh() = default;
		
	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CEnvMesh> Create(const std::shared_ptr<FEnvMeshDesc>& _spEnvMeshDesc);

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
	};
}
