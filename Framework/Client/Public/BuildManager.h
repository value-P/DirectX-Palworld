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
		/* �Ͽ�¡ ��� ��� */
		HRESULT AddHousingLump(uint32 _iObjectID, std::shared_ptr<CHousingLump> _spHousingLumps);

		/* �Ͽ�¡ ��� ���� */
		HRESULT RemoveHousingLump(uint32 _iObjectID);

		/* �Ͽ�¡ ��� �˻� */
		std::shared_ptr<CHousingLump> FindHousingLump(uint32 _iObjectID);

		/* �Ͽ�¡ ������ �˻� */
		std::shared_ptr<CArchitecturePrefab> FindHousingPrefab(EArchitectureType _eArchitectureType);

		/* ��ġ�� ���๰ ��Ͽ��� ���๰ ���� */
		HRESULT RemoveArchitecture(EArchitectureType _eArchitectureType, std::shared_ptr<CArchitecture> _spArchitecture);

		/* ���࿡ �ʿ��� ��� �˻� */
		const std::optional<pair<std::shared_ptr<CMineral>, uint32>> GetArchitectureNeededMinerals(EArchitectureType _eArchitectureType, uint32 _iIndex);
	#pragma endregion

	public:
	#pragma region �Ͽ�¡
		/* �Ͽ�¡ ���� ��� Ȱ��ȭ */
		HRESULT OnBuildMode(EArchitectureType _eArchitectureType);
		HRESULT OffBuildMode();
		HRESULT OnDestroyMode();
		HRESULT OffDestroyMode();
		void BuildInput();
		void DestroyInput();
		void SaveBowlingAlley(); // �ӽ�

		/* �Ͽ�¡ ������ ���� ���� */
		void ChangeHousingPrefab();

		/* ���� �ȿ� �������� ���� �Ͽ�¡�� �ִ��� �˻��մϴ�. */
		std::shared_ptr<CHousing> FindNearHousing(const Vector3& _vPos, float _fRange);
		void FindRecursiveCombinedHousing(std::shared_ptr<CHousing>& _spNearestHousing, std::shared_ptr<CHousing> _spHousing, const Vector3& _vPrefabPos, float _fRange, float* _pNearest);

	#pragma endregion

		/* ���� ���� */
		HRESULT AssignBaseCamp(std::shared_ptr<CPalBox> _spPalBox);
		void SetBaseCampNull() { m_spPalBox = nullptr; }

		/* ���� ���� �˻� */
		bool IsExistBaseCamp();
		bool IsInBaseCamp(const Vector3& _vBuildPos);

		/* ���� ��� ť�� ���๰ �߰� */
		void AddArchitectureToWaitQue(const EArchitectureType& _eArchitectureType) { m_queBuildWait.push(_eArchitectureType); };

		/* ���� ��� ť�� ���๰�� ���� ���, �����մϴ�. */
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

		// 24-05-13 �̵���
		Vector3 GetPalBoxPos(); 


	/********************
		Data Members
	********************/
	private:
		bool m_bBuildMode = false;
		bool m_bDestroyMode = false;

		/* [����] */
		std::shared_ptr<CPalBox> m_spPalBox = nullptr;

		/* [�Ͽ�¡ ���] - ������ƮID�� Ű ������ �����մϴ�. */
		std::unordered_map<uint32, std::shared_ptr<CHousingLump>> m_umapHousingLumps;

		/* [���๰ ������] */
		std::unordered_map<EArchitectureType, std::shared_ptr<CArchitecturePrefab>> m_umapArchitecturePrefabs;

		/* [���� ���õ� ���๰ ������] */
		std::shared_ptr<CArchitecturePrefab> m_spCurArchitecturePrefab = nullptr;

		/* [������ ����� �ı� ���� ���๰] */
		std::weak_ptr<CArchitecture> m_wpDestroyExpectedArchitecture;

		/* [���� ��� ť] */
		std::queue<EArchitectureType> m_queBuildWait;

		/* [��ġ�� ���๰] */
		std::unordered_map<EArchitectureType, std::vector<std::shared_ptr<CArchitecture>>> m_umapBuiltArchitectures;

	private:
		/* [��ǲ �Ŵ���] */
		std::weak_ptr<CInputManager> m_wpInputManager;

		/* [ī�޶� �Ŵ���] */
		std::weak_ptr<CCameraManager> m_wpCameraManager;

		/* [������ �Ŵ���] */
		std::weak_ptr<CPhysXManager> m_wpPhysXManager;

		/* [UI �Ŵ���] */
		std::weak_ptr<CUIManager> m_wpUIManager;

	private:
		/* [�κ��丮 �ý���]*/
		std::weak_ptr<CInventorySystem> m_wpInventorySystem;
	};
}
