#pragma once
#include "ArchitecturePrefab.h"

namespace Engine
{
	class CMeshComponent;
	class CBoxComponent;
}

namespace Client
{
	class CHousing;

	struct FHousingPrefabDesc final : public FArchitecturePrefabDesc
	{
		EBuildType m_eBuildType = EBuildType::ENUM_END;

		std::unordered_map<EBuildType, EBuildType> m_umapNeededBuildTypes;
	};

	class CHousingPrefab : public CArchitecturePrefab
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CHousingPrefab() = default;

		virtual ~CHousingPrefab() = default;
		
	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CHousingPrefab> Create(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc);

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
		/* 가까이 있는 하우징에 붙습니다. */
		void AttachToNearHousing();
		EBuildDir DecideBuildDir(const EBuildType& _eBuildType, const EBuildType& _eOpponentBuildType, float _fAngle);

		/* 현재 해당 프리팹에 해당하는 건축물이 건축 가능한 상태인지 조건을 체크합니다. */
		virtual bool IsBuildable() override;
		bool IsBuildable(EBuildType _eBuildType);
		void SetNoneAttached();

	/********************
		Getter/Setter
	********************/
	public:
		const EBuildType& GetBuildType() { return m_eBuildType; }
		const std::unordered_map<EBuildType, EBuildType>& GetNeededBuildTypes() { return m_umapNeededBuildTypes; }
		bool GetIsAttached() { return m_bAttached; }
		std::shared_ptr<CHousing> GetAttachedHousing()
		{ 
			if (!m_wpAttachedHousing.expired()) { return m_wpAttachedHousing.lock(); }
			else { return nullptr; }
		}
		const std::tuple<int32, int32, int32>& GetBuildIndex() { return m_tpBuildIndex; }
		int32 GetFloor() { return m_iFloor; }
		const EBuildDir& GetBuildDir() { return m_eBuildDir; }

	/********************
		Data Members
	********************/
	protected:
		/* 하우징 빌드 타입 */
		EBuildType m_eBuildType = EBuildType::ENUM_END;

		/* 하우징이 건축되려면 필요한 상대 하우징의 빌드 타입들 */
		std::unordered_map<EBuildType, EBuildType> m_umapNeededBuildTypes;

		/* Attach 여부 */
		bool m_bAttached = false;

		/* Attach될 때 등록될 하우징 포인터 */
		std::weak_ptr<CHousing> m_wpAttachedHousing;

		/* 프리팹이 건축될 방향 */
		EBuildDir m_eBuildDir = EBuildDir::ENUM_END;

		/* 프리팹이 건축될 인덱스 */
		std::tuple<int32, int32, int32> m_tpBuildIndex = tuple(0, 0, 0);

		/* 층 */
		int32 m_iFloor = 0;
	};
}
