#pragma once
#include "Factory.h"

namespace Engine
{
	class CPanel;
}

namespace Client
{
	struct FWorkBenchDesc : public FFactoryDesc
	{
	};

	class CWorkBench final : public CFactory
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CWorkBench() = default;

		virtual ~CWorkBench() = default;

	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CWorkBench> Create(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc);

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

		virtual void AddItemToFactory(EItemType _eItemType, int32 _iNumItems) override;

		virtual void RemoveItemFromFactory() override;

	public:
		void SetWatingItemPanel(const shared_ptr<CPanel>& _spWaitingPanel) { m_spWaitingItemPanel = _spWaitingPanel; }

	/********************
		Data Members
	********************/
	private:
		// F�� ������ �ʾ��� ���
		// �䱸 ���� �г�
		std::shared_ptr<CPanel> m_spRequiredAptitudePanel = nullptr;

		// ��� ���� ������ ���� �г�
		std::shared_ptr<CPanel> m_spWaitingItemIsNullPanel = nullptr;

		// ��� ������ �г�
		std::shared_ptr<CPanel> m_spWaitingItemPanel = nullptr;

		// ������ ���� �г�
		std::shared_ptr<CPanel> m_spSelectRecipePanel = nullptr;
		
		// F�� ������ ���
		// �۾� �г�
		std::shared_ptr<CPanel> m_spWorkingWindow = nullptr;
	};
}
