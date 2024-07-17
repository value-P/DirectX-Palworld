#pragma once
#include "Factory.h"

namespace Engine
{
	class CPanel;
}

namespace Client
{
	struct FSphereTableDesc : public FFactoryDesc
	{
	};

	class CSphereTable final : public CFactory
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CSphereTable() = default;

		virtual ~CSphereTable() = default;

	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CSphereTable> Create(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc);

		virtual HRESULT Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc) override;

		virtual HRESULT BeginPlay() override;

		virtual int32 PreTick(float _fDeltaSeconds) override;

		virtual int32 Tick(float _fDeltaSeconds) override;

		virtual int32 PostTick(float _fDeltaSeconds) override;

		virtual HRESULT EndPlay() override;

		virtual void Release() override;
		
	/********************
		Methods
	********************/
	private:
		virtual HRESULT InitializeMeshComponent() override;

		virtual HRESULT InitializeColliderComponent() override;

	/********************
		Data Members
	********************/
	protected:
	#pragma region UI
		/* == F를 누르지 않았을 때 == */
		// 패널 : 버튼 선택
		// [F] : 자기
		std::shared_ptr<CPanel> m_spSelectPanel = nullptr;

	#pragma endregion
	protected:

	};
}

