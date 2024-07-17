#pragma once
#include "Architecture.h"

namespace Engine
{
	class CPanel;
}

namespace Client
{
	class CChestBoxSystem;

	struct FChestDesc : public FArchitectureDesc
	{
	};

	class CChest final : public CArchitecture
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CChest() = default;

		virtual ~CChest() = default;

	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CChest> Create(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc);

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
	public:
		virtual void RecycleFromPool() { return; }

		virtual void ReturnToPool() { return; }

	private:
		virtual HRESULT InitializeMeshComponent() override;

		virtual HRESULT InitializeColliderComponent() override;

	public:
		void AddItemToWoodenChest(EItemType _eItemType, int32 _iNumItems);

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
		// �г� : ���� ������ ���
		std::shared_ptr<CPanel> m_spChestWindow = nullptr;

	private:
		std::weak_ptr<CChestBoxSystem> m_wpChestBoxSystem;
	};
}
