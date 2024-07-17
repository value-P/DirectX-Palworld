#pragma once
#include "Manager.h"

namespace Engine
{
	class CInputManager;
	class CCameraManager;
	class CPhysXManager;
}

namespace Client
{
	class CHousingLump;
	class CHousing;
	class CHousingPrefab;
	class CArchitecture;
	class CArchitecturePrefab;
	class CMineral;
	
	class CPalBox;

	class CUIManager;
	class CInventorySystem;

	class CBuildManager final : public CManager
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		DECLARE_SINGLETON(CBuildManager)

	/********************
		Events
	********************/
	public:
		HRESULT Initialize();
		HRESULT BeginPlay();
		int32 Tick(float _fDeltaSeconds);
		virtual void Release() override;

	/********************
		Methods
	********************/
	public:
	#pragma region Initialize

		HRESULT InitializeHousingPrefabs();
		HRESULT InitializeNormalArchitecturePrefabs();

	#pragma endregion

	public:
	#pragma region Default
		/* 하우징 덩어리 등록 */
		HRESULT AddHousingLump(uint32 _iObjectID, std::shared_ptr<CHousingLump> _spHousingLumps);

		/* 하우징 덩어리 삭제 */
		HRESULT RemoveHousingLump(uint32 _iObjectID);

		/* 하우징 덩어리 검색 */
		std::shared_ptr<CHousingLump> FindHousingLump(uint32 _iObjectID);

		/* 하우징 프리팹 검색 */
		std::shared_ptr<CArchitecturePrefab> FindHousingPrefab(EArchitectureType _eArchitectureType);

		/* 설치된 건축물 목록에서 건축물 삭제 */
		HRESULT RemoveArchitecture(EArchitectureType _eArchitectureType, std::shared_ptr<CArchitecture> _spArchitecture);

		/* 건축에 필요한 재료 검색 */
		const std::optional<pair<std::shared_ptr<CMineral>, uint32>> GetArchitectureNeededMinerals(EArchitectureType _eArchitectureType, uint32 _iIndex);
	#pragma endregion

	public:
	#pragma region 하우징
		/* 하우징 빌드 모드 활성화 */
		HRESULT OnBuildMode(EArchitectureType _eArchitectureType);
		HRESULT OffBuildMode();
		HRESULT OnDestroyMode();
		HRESULT OffDestroyMode();
		void BuildInput();
		void DestroyInput();
		void SaveBowlingAlley(); // 임시

		/* 하우징 프리팹 종류 변경 */
		void ChangeHousingPrefab();

		/* 범위 안에 프리팹을 붙일 하우징이 있는지 검색합니다. */
		std::shared_ptr<CHousing> FindNearHousing(const Vector3& _vPos, float _fRange);
		void FindRecursiveCombinedHousing(std::shared_ptr<CHousing>& _spNearestHousing, std::shared_ptr<CHousing> _spHousing, const Vector3& _vPrefabPos, float _fRange, float* _pNearest);

	#pragma endregion

		/* 거점 설정 */
		HRESULT AssignBaseCamp(std::shared_ptr<CPalBox> _spPalBox);
		void SetBaseCampNull() { m_spPalBox = nullptr; }

		/* 거점 범위 검사 */
		bool IsExistBaseCamp();
		bool IsInBaseCamp(const Vector3& _vBuildPos);

		/* 건축 대기 큐에 건축물 추가 */
		void AddArchitectureToWaitQue(const EArchitectureType& _eArchitectureType) { m_queBuildWait.push(_eArchitectureType); };

		/* 건축 대기 큐에 건축물이 들어올 경우, 건축합니다. */
		void MonitoringWaitQue();
			HRESULT BuildArchitecture(EArchitectureType _eArchitextureType);
			EBuildDir DecideOpponentBuildDir(EBuildDir _eArchitextureType);

	public:
		const std::vector<std::shared_ptr<CArchitecture>>& GetBuildtArchitectures(EArchitectureType _eArchitectureType) { return m_umapBuiltArchitectures[_eArchitectureType]; }

	/********************
		Getter/Setter
	********************/
	public:
		const std::unordered_map<uint32, std::shared_ptr<CHousingLump>>& GetHousingLumps() { return m_umapHousingLumps; }

		std::shared_ptr<CArchitecturePrefab> GetCurArchitecturePrefab() { return m_spCurArchitecturePrefab; }

		bool IsBuildMode() { return m_bBuildMode || m_bDestroyMode || !m_queBuildWait.empty(); }

		bool IsArchitectureConstructed(EArchitectureType _eArchitectureType);

		// 24-05-13 이동준
		Vector3 GetPalBoxPos(); 


	/********************
		Data Members
	********************/
	private:
		bool m_bBuildMode = false;
		bool m_bDestroyMode = false;

		/* [거점] */
		std::shared_ptr<CPalBox> m_spPalBox = nullptr;

		/* [하우징 덩어리] - 오브젝트ID를 키 값으로 저장합니다. */
		std::unordered_map<uint32, std::shared_ptr<CHousingLump>> m_umapHousingLumps;

		/* [건축물 프리팹] */
		std::unordered_map<EArchitectureType, std::shared_ptr<CArchitecturePrefab>> m_umapArchitecturePrefabs;

		/* [현재 선택된 건축물 프리팹] */
		std::shared_ptr<CArchitecturePrefab> m_spCurArchitecturePrefab = nullptr;

		/* [광선에 검출된 파괴 예정 건축물] */
		std::weak_ptr<CArchitecture> m_wpDestroyExpectedArchitecture;

		/* [건축 대기 큐] */
		std::queue<EArchitectureType> m_queBuildWait;

		/* [설치된 건축물] */
		std::unordered_map<EArchitectureType, std::vector<std::shared_ptr<CArchitecture>>> m_umapBuiltArchitectures;

	private:
		/* [인풋 매니저] */
		std::weak_ptr<CInputManager> m_wpInputManager;

		/* [카메라 매니저] */
		std::weak_ptr<CCameraManager> m_wpCameraManager;

		/* [피직스 매니저] */
		std::weak_ptr<CPhysXManager> m_wpPhysXManager;

		/* [UI 매니저] */
		std::weak_ptr<CUIManager> m_wpUIManager;

	private:
		/* [인벤토리 시스템]*/
		std::weak_ptr<CInventorySystem> m_wpInventorySystem;
	};
}
