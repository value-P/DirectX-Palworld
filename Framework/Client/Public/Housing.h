#pragma once
#include "Architecture.h"

namespace Client
{

	class CHousing;
	class CHousingLump;

	struct FHousingDesc : public FArchitectureDesc
	{
		EBuildType m_eBuildType = EBuildType::ENUM_END;

		EBuildDir m_eBuildDir = EBuildDir::ENUM_END;

		std::unordered_map<EBuildType, EBuildType> m_umapNeededBuildTypes;

		std::shared_ptr<CHousingLump> m_spHousingLump = nullptr;

		std::tuple<int32, int32, int32> m_tpBuildIndex = tuple(0, 0, 0);

		int32 m_iFloor = 0;
	};

	class CHousing : public CArchitecture
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CHousing() = default;

		virtual ~CHousing() = default;
		
	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CHousing> Create(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc);

		virtual HRESULT Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc) override;

		virtual HRESULT BeginPlay() override;

		virtual int32 PreTick(float _fDeltaSeconds) override;

		virtual int32 Tick(float _fDeltaSeconds) override;
		
		virtual int32 PostTick(float _fDeltaSeconds) override;

		virtual HRESULT EndPlay() override;

		virtual void Release() override;

	public:
		virtual void RecycleFromPool() override;

		virtual void ReturnToPool() override;

	/********************
		Methods
	********************/
	public:
		/* 건축 방향에 하우징 추가 */
		HRESULT AddCombinedHousing(EBuildDir _eBuildDir, std::shared_ptr<CHousing> _spHousing);

		/* 건축 방향에 연결된 하우징 검색 */
		std::shared_ptr<CHousing> GetCombinedHousing(EBuildDir _eBuildDir);

		/* 파괴 */
		virtual void DestroyArchitecture() override;

		// 임시
		HRESULT SaveHousing(rapidjson::PrettyWriter<rapidjson::StringBuffer>& _writer);

	private:
		virtual HRESULT InitializeMeshComponent() override;
		virtual HRESULT InitializeColliderComponent() override;
		
	/********************
		Getter/Setter
	********************/
	public:
		/* 빌드 타입 */
		const EBuildType& GetBuildType() const { return m_eBuildType; }

		/* 건축 방향 */
		const EBuildDir& GetBuildDir() const { return m_eBuildDir; }

		/* 이 하우징이 건축되려면 필요한 상대 하우징의 빌드 타입들 */
		const std::unordered_map<EBuildType, EBuildType>& GetNeededBuildTypes() { return m_umapNeededBuildTypes; }

		/* 하우징 덩어리 */
		std::shared_ptr<CHousingLump> GetHousingLump() { CHECKF(!m_wpHousingLump.expired(), L"m_wpHousingLump is expired : CHousing"); return m_wpHousingLump.lock(); }

		/* 부모 하우징 */
		void SetParentHousing(EBuildDir _eBuildDir, std::shared_ptr<CHousing> _spHousing) { m_prParentHousing.first = _eBuildDir; m_prParentHousing.second = _spHousing; }
		std::optional<pair<EBuildDir, std::shared_ptr<CHousing>>> GetParentHousing()
		{
			if (!m_prParentHousing.second) { return m_prParentHousing; }
			else { return nullopt; }
		}

		/* 하우징에 연결되어 지어진 자식 하우징들 */
		std::vector<std::shared_ptr<CHousing>>& GetCombinedHousings() { return m_vecCombinedHousings; }

		/* 빌드 인덱스 */
		const tuple<int32, int32, int32>& GetBuildIndex() { return m_tpBuildIndex; }

		/* 층 */
		int32 GetFloor() { return m_iFloor; }

		const wstring& GetHousingModelFilePath() { return m_wstrModelFilePath; };

	/********************
		Data Members
	********************/
	protected:
		/* 빌드 타입 */
		EBuildType m_eBuildType = EBuildType::ENUM_END;

		/* 건축 방향 */
		EBuildDir m_eBuildDir = EBuildDir::ENUM_END;

		/* 이 하우징이 건축되려면 필요한 상대 하우징의 빌드 타입들 */
		std::unordered_map<EBuildType, EBuildType> m_umapNeededBuildTypes;

		/* 하우징 덩어리 */
		std::weak_ptr<CHousingLump> m_wpHousingLump;

		/* 부모 하우징에 연결된 방향 */
		std::pair<EBuildDir, std::shared_ptr<CHousing>> m_prParentHousing;

		/* 이 하우징에 연결되어 지어진 하우징들 */
		std::vector<std::shared_ptr<CHousing>> m_vecCombinedHousings;

		/* 최초의 토대(0, 0, 0) 기준 3차원 벡터 상의 인덱스 위치 */
		std::tuple<int32, int32, int32> m_tpBuildIndex = tuple(0, 0, 0);

		/* 층 */
		int32 m_iFloor = 0;
	};
}
