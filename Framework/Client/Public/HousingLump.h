#pragma once
#include "GameObject.h"

namespace Client
{
	class CBuildManager;
	class CHousing;

	struct FHousingLumpDesc : public CGameObject::FGameObjectDesc
	{
		
	};

	class CHousingLump final : public CGameObject
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CHousingLump() = default;

		virtual ~CHousingLump() = default;
		
	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CHousingLump> Create(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc);

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
	#pragma region Default
		/* 하우징 추가 */
		HRESULT AddHousing(std::tuple<int32, int32, int32> _tpIndex, std::shared_ptr<CHousing> _spHousing);

		/* 하우징 삭제 */
		HRESULT RemoveHousing(std::tuple<int32, int32, int32> _tpIndex);

		/* 하우징 검색 */
		std::shared_ptr<CHousing> FindHousing(std::tuple<int32, int32, int32> _tpIndex);

		/* 하우징 저장 */
		HRESULT SaveHousingLump();
		// HRESULT SaveHousing(rapidjson::PrettyWriter<rapidjson::StringBuffer>& _writer, std::shared_ptr<CHousing> _spHousing);
	#pragma endregion

	public:
		/* 설치할 인덱스 위치가 빌드 타입에 맞는 지지대의 원리를 만족하는가? */
		bool IsSatisfiedSupportingTheory(std::tuple<int32, int32, int32> _tpIndex, EBuildType _eBuildType);

	/********************
		Getter/Setter
	********************/
	public:
		const std::unordered_map <std::tuple<int32, int32, int32>, std::shared_ptr<CHousing>, FHashableTuple>& GetBuildIndices() { return m_umapBuildIndices; }

		std::shared_ptr<CHousing> GetFirstFoundation() { return m_umapBuildIndices[std::tuple(0, 0, 0)]; }

	/********************
		Data Members
	********************/
	private:
		/* 토대 개수 */
		uint32 m_iNumFoundation = 0;

		/* 3차원 인덱스 벡터를 키 값으로 가지는 umap */
		std::unordered_map <std::tuple<int32, int32, int32>, std::shared_ptr<CHousing>, FHashableTuple> m_umapBuildIndices;

	private:
		/* 빌드 매니저 */
		shared_ptr<CBuildManager> m_spBuildManager = nullptr;
	};
}
