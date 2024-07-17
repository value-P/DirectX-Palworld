#pragma once
#include "GameObject.h"

namespace Engine
{
	class CTriangleMeshComponent;
}

namespace Client
{
	struct FEnvCollisionDesc : public CGameObject::FGameObjectDesc
	{
		std::wstring m_wstrEnvCollisionFilePath = L"";

		bool m_bInsertDirectly = true;
	};

	class CEnvCollision final : public CGameObject
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CEnvCollision() = default;

		virtual ~CEnvCollision() = default;
		
	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CEnvCollision> Create(const std::shared_ptr<FEnvCollisionDesc>& _spEnvCollisionDesc);

		virtual HRESULT Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc);

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
		HRESULT ActivateEnvCollision();

		HRESULT DeactivateEnvCollision();

	/********************
		Data Members
	********************/
	private:
		std::shared_ptr<CTriangleMeshComponent> m_spTriangleMeshComponent = nullptr;
	};
}
