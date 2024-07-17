#pragma once
#include "GameObject.h"

namespace Engine
{
	class CCameraComponent;
	class CMeshComponent;

	/********************
		Ctor/Dtor
	********************/
	struct FCharacterDesc : public CGameObject::FGameObjectDesc
	{
		float m_fMoveSpeed = 10.0f;

		float m_fJumpSpeed = 10.0f;
	};

	class ENGINE_DLL CCharacter : public CGameObject
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CCharacter() = default;

		virtual ~CCharacter() = default;
		
	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CCharacter> Create(const std::shared_ptr<FCharacterDesc>& _spCharacterDesc);

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
		shared_ptr<CCameraComponent> GetCameraComponent() const { return m_spCameraComponent; }

		shared_ptr<CMeshComponent> GetMeshComponent() const { return m_spMeshComponent; }

		float GetMoveSpeed() const { return m_fMoveSpeed; }

		float GetJumpSpeed() const { return m_fJumpSpeed; }
		
	/********************
		Data Members
	********************/
	protected:
		std::shared_ptr<CCameraComponent> m_spCameraComponent = nullptr;

		std::shared_ptr<CMeshComponent> m_spMeshComponent = nullptr;

	protected:
		float m_fJumpSpeed = 10.0f;

		float m_fMoveSpeed = 10.0f;
	};
}
