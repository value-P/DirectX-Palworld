#pragma once
#include "Habitat.h"

namespace Engine
{
	class CSoundManager;
	class CBoxComponent;
	class CPanel;
	class CTextComponent;
	class CImageComponent;
	class CProgressBarComponent;
}

namespace Client
{
	class CBuildManager;
	class CInventorySystem;
	class CChestBoxSystem;
	class CPlayer;
	class CMineral;
	class CMonster;
	class CUIManager;

	struct FArchitectureDesc : public FHabitatDesc
	{
		EArchitectureType m_eArchitectureType = EArchitectureType::ENUM_END;

		int32 m_iArchitectureUnlockLevel = 0;

		std::wstring m_wstrShaderName = L"";

		FBuildStatus m_tBuildStatus = {};

		Vector3 m_vColliderExtents = Vector3(1.0f);

		Vector3 m_vColliderCenter = Vector3(0.0f);

		Vector3 m_vColliderRotate = Vector3(0.0f);

		float m_fPivotScaleRatio = 0.0001f;

		float m_fInteractionRadius = 0.0f;

		std::vector<std::shared_ptr<CMineral>> m_vecNeededMinerals;

		std::vector<uint32> m_vecNeededMineralCounts;
	};

	class CArchitecture abstract : public CHabitat
	{
	private:
		struct FProductionArchitectureInfo
		{
			// 부모 패널
			shared_ptr<CTextComponent> spSecTextComponent = nullptr;
			shared_ptr<CProgressBarComponent> spProgressComponent = nullptr;
			// 자식 패널
			shared_ptr<CPanel> spChildPanel = nullptr;
			shared_ptr<CImageComponent> spPalImageComponent = nullptr;
			shared_ptr<CProgressBarComponent> spPalProgressComponent = nullptr;

			Vector4 vWhiteColor = { 1.0f, 1.0f, 1.0f, 1.0f };
			Vector4 vGrayColor = { 0.7f, 0.7f, 0.7f, 1.0f };
		};

	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CArchitecture() = default;

		virtual ~CArchitecture() = default;
		
	/********************
		Events
	********************/
	public:
		virtual HRESULT Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc) override;

		virtual HRESULT BeginPlay() override;

		virtual int32 PreTick(float _fDeltaSeconds) override;

		virtual int32 Tick(float _fDeltaSeconds) override;
		
		virtual int32 PostTick(float _fDeltaSeconds) override;

		virtual HRESULT EndPlay() override;

		virtual void Release() override;
		
	public:
		virtual void OnTriggerEnter(CComponent* _pSrcComponent, CComponent* _pDstComponent);

		virtual void OnTriggerExit(CComponent* _pSrcComponent, CComponent* _pDstComponent);

	/********************
		Methods
	********************/
	public:
		virtual void RecycleFromPool() { return; }

		virtual void ReturnToPool() { return; }

	public:
		virtual HRESULT InitializeMeshComponent() = 0;

		virtual HRESULT InitializeColliderComponent() = 0;

	public:
		virtual bool BuildArchitecture(float _fDeltaSeconds);

		virtual void DestroyArchitecture();

	public:
		void UpdateUI();

		void AddItemToInventory(EItemType _eItemType, int32 _iNumItems);

		virtual void AddItemToFactory(EItemType _eItemType, int32 _iNumItems);

		void OnBluePrint();
		void OffBluePrint();

		void PlayBuildCompleteSound(EArchitectureType _eArchitectureType);

		virtual void ClearEffect() {};

	/********************
		Getter/Setter
	********************/
	public:
		const Vector3& GetColliderExtents() { return m_vColliderExtents; }
		const Vector3& GetColliderCenter() { return m_vColliderCenter; }
		const Vector3& GetColliderRotate() { return m_vColliderRotate; }

		void SetComplete() { m_bCompleted = true; }

		float GetPivotScaleRatio() const { return m_fPivotScaleRatio; }

		/* 해금 레벨 */
		const int32 GetArchitectureUnlockLevel() const { return m_iArchitectureUnlockLevel; }
		
		void SetArchitectureUnlockLevel(int32 _iArchitectureUnlockLevel) { m_iArchitectureUnlockLevel = _iArchitectureUnlockLevel; }

		/* 건축물 이름 */
		const EArchitectureType GetArchitectureType() const { return m_eArchitectureType; }

		void SetArchitectureType(EArchitectureType& _eCurArchitectureType) { m_eArchitectureType = _eCurArchitectureType; }

		/* 필요 재료 목록 */
		const std::vector<std::shared_ptr<CMineral>> GetNeededMinerals() { return m_vecNeededMinerals; }

		void SetNeededMinerals(const std::vector<std::shared_ptr<CMineral>>& _vecNeededMinerals) { m_vecNeededMinerals = _vecNeededMinerals; }

		/* 필요 재료 개수 */
		const std::vector<uint32> GetNeededMineralCounts() { return m_vecNeededMineralCounts; }

		void SetNeededMineralCounts(const std::vector<uint32>& _vecNeededMineralCounts) { m_vecNeededMineralCounts = _vecNeededMineralCounts; }

		/* 필요 재료, 개수 벡터의 사이즈 반환 */
		uint32 GetNeededMineralsVecSize() { return static_cast<uint32>(m_vecNeededMinerals.size()); }
		uint32 GetNeededMineralsCountsVecSize() { return static_cast<uint32>(m_vecNeededMineralCounts.size()); }

		bool IsBuildComplete() { return m_tBuildStatus.m_bBuildCompleted; }
		bool IsFactory() { return m_bIsFactory; }
		bool IsHousing() { return m_eArchitectureType >= EArchitectureType::WOODEN_FOUNDATION && m_eArchitectureType <= EArchitectureType::WOODEN_SLOPE_ROOF; }

	/********************
		Data Members
	********************/
	protected:
		std::weak_ptr<CSoundManager> m_wpSoundManager;

		std::shared_ptr<CBuildManager> m_spBuildManager = nullptr;

		std::weak_ptr<CInventorySystem> m_wpInventorySystem;

		std::weak_ptr<CChestBoxSystem> m_wpChestBoxSystem;

	protected:
		std::shared_ptr<CBoxComponent> m_spBoxComponent = nullptr;

		Vector3 m_vColliderExtents = Vector3(1.0f);
		
		Vector3 m_vColliderCenter = Vector3(0.0f);
		
		Vector3 m_vColliderRotate = Vector3(0.0f);

		// 피벗 스케일
		float m_fPivotScaleRatio = 0.0001f;

	protected:
		// 이펙트 셰이더
		std::wstring m_wstrShaderName = L"";

		// 모델 파일 경로
		std::wstring m_wstrModelFilePath = L"";

	protected:
		bool m_bPrefab = false;

		bool m_bCompleted = false;

		// 건축물 해금 레벨
		int32 m_iArchitectureUnlockLevel = 0;

		// 건축물 이름
		EArchitectureType m_eArchitectureType = EArchitectureType::ENUM_END;

		// 건축하는데 필요한 재료의 목록
		std::vector<std::shared_ptr<CMineral>> m_vecNeededMinerals;

		// 건축하는데 필요한 재료의 개수
		std::vector<uint32> m_vecNeededMineralCounts;

	protected:
		// 현재 건설 중인 건물 현황
		FBuildStatus m_tBuildStatus{};

		// 상호 작용 플래그
		bool m_bInteractionEnable = false;

		bool m_bIsFactory = false;

	protected:
		// 건축 제작 ui
		shared_ptr<CPanel> m_spProductionPanel = nullptr;

		// 건축 제작 ui에서 변경되어야하는 컴포넌트
		FProductionArchitectureInfo m_tPanelInfo;

	protected:
		std::weak_ptr<CUIManager> m_wpUIManager;
	};
}
