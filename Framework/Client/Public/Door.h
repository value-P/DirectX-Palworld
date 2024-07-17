#pragma once
#include "Housing.h"

namespace Engine
{
	class CSoundManager;
	class CBoxComponent;
}

namespace Client
{
	struct FDoorDesc : public FHousingDesc
	{

	};

	class CDoor final : public CHousing
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CDoor() = default;

		virtual ~CDoor() = default;
		
	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CDoor> Create(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc);

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
		HRESULT InitializeDoorParts();
		HRESULT InitializeColliders();
		
	public:
		/* 문 베이스와 문 파츠의 SRT를 동기화합니다. */
		void SynchronizationDoorPartsSRT();

		/* 플레이어와 거리를 감지해서 상호작용을 결정합니다. */
		void DoorInteraction();

	/********************
		Getter/Setter
	********************/
	public:
		

	/********************
		Data Members
	********************/
	protected:
		/* 진짜 문 */
		std::shared_ptr<CHousing> m_spDoorParts = nullptr;

		/* 오픈 여부 */
		bool m_bDoorOpend = false;

		/* 문을 막기 위한 콜라이더 */
		std::shared_ptr<CBoxComponent> m_spCenterBoxComponent = nullptr;
		std::shared_ptr<CBoxComponent> m_spLeftBoxComponent = nullptr;
		std::shared_ptr<CBoxComponent> m_spRightBoxComponent = nullptr;

	protected:
		std::weak_ptr<CSoundManager> m_wpSoundManager;
	};
}
