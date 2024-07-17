#pragma once
#include "Architecture.h"

namespace Engine
{
	class CPanel;
}

namespace Client
{
	struct FRepairBenchDesc : public FArchitectureDesc
	{

	};

	class CRepairBench final : public CArchitecture
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CRepairBench() = default;

		virtual ~CRepairBench() = default;

	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CRepairBench> Create(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc);

		virtual HRESULT Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc) override;

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
	private:
		virtual HRESULT InitializeMeshComponent() override;

		virtual HRESULT InitializeColliderComponent() override;

	/********************
		Data Members
	********************/
	private:
		/* == F�� ������ �ʾ��� �� == */
		// �г� : ��ư ����
		// [F] : ����
		// [V] : ���� ����
		// [C] : ���� ����
		// [4] : ����
		std::shared_ptr<CPanel> m_spSelectPanel = nullptr;
		
		/* == F�� ������ �� == */
		// �г� : �κ��丮 -> �Ƹ� �κ� �ý��ۿ��� �����ͼ� ��� ������ ������ ����
		// std::shared_ptr<CPanel> m_spInventoryWindow = nullptr;
		// 
		// �г� : ���� ������ ���
		std::shared_ptr<CPanel> m_spRepairWindow = nullptr;
	};
}
