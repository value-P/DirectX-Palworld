#pragma once
#include "Architecture.h"

namespace Engine
{
	class CPanel;
}

namespace Client
{
	struct FRanchDesc final : public FArchitectureDesc
	{
	};

	class CRanch final : public CArchitecture
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CRanch() = default;

		virtual ~CRanch() = default;

	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CRanch> Create(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc);

		virtual HRESULT Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc) override;

		virtual HRESULT BeginPlay() override;

		virtual int32 PreTick(float _fDeltaSeconds) override;

		virtual int32 Tick(float _fDeltaSeconds) override;

		virtual int32 PostTick(float _fDeltaSeconds) override;

		virtual HRESULT EndPlay() override;

		virtual void Release() override;
		
	/********************
		Mehtods
	********************/
	public:
		virtual void RecycleFromPool() { return; }

		virtual void ReturnToPool() { return; }

	private:
		virtual HRESULT InitializeMeshComponent() override;

		virtual HRESULT InitializeColliderComponent() override;
		
	/********************
		Getter/Setter
	********************/
	public:
		void CompleteQuest();
	};
}

