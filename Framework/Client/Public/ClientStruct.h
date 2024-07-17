
#pragma once

namespace Client
{
	struct FHashableTuple
	{
		// 해시 결합 함수
		size_t operator()(const std::tuple<int, int, int>& _tp) const
		{
			size_t h1 = std::hash<int>()(get<0>(_tp));
			size_t h2 = std::hash<int>()(get<1>(_tp));
			size_t h3 = std::hash<int>()(get<2>(_tp));

			return h1 ^ (h2 << 1) ^ (h3 << 2);
		}
	};

	/********************
		Habitat Cache
	********************/
	struct FHabitatCache
	{
		// habitat desc
		int32 m_iHabitatType = 0;

		// game object desc(transform component desc)
		Vector3 m_vScale = Vector3(1.0f, 1.0f, 1.0f);
		Vector3 m_vRotate = Vector3(0.0f, 0.0f, 0.0f);
		Vector3 m_vPosition = Vector3(0.0f, 0.0f, 0.0f);
		std::tuple<int32, int32, int32> m_tpOctreeNodeIndex = { 0, 0, 0 };

		// mesh component desc
		std::wstring m_wstrModelFilePath = L"";

		// collider component desc
		bool m_bTrigger = false;
		uint32 m_iColliderType = 0;
	};

	struct FNatureCache : public FHabitatCache
	{
		int32 m_iNatureType = 0;
	};

	struct FTreeCache : public FNatureCache
	{
		// tree desc
		float m_fCurHp = 100.0f;

		// capsule component desc
		Vector3 m_vCenter = Vector3(0.0f, 0.0f, 0.0f);
		Vector3 m_vExtents = Vector3(1.0f, 1.0f, 1.0f);
	};

	struct FRockCache : public FNatureCache
	{
		// rock desc
		float m_fCurHp = 100.0f;

		// sphere component desc
		Vector3 m_vCenter = Vector3(0.0f, 0.0f, 0.0f);
		float m_fRadius = 1.0f;
	};

	struct FBranchCache : public FNatureCache
	{
		// capsule component desc
		Vector3 m_vCenter = Vector3(0.0f, 0.0f, 0.0f);
		float m_fRadius = 1.0f;
	};

	struct FStoneCache : public FNatureCache
	{
		// sphere component desc
		Vector3 m_vCenter = Vector3(0.0f, 0.0f, 0.0f);
		float m_fRadius = 1.0f;
	};

	struct FPalCache : public FNatureCache
	{
		// sphere component desc
		Vector3 m_vCenter = Vector3(0.0f, 0.0f, 0.0f);
		float m_fRadius = 0.001f;
		EMonsterType m_eMonsterType = EMonsterType::ENUM_END;
	};

	// 동연 리뷰
	struct FArchitectureCache : public FHabitatCache
	{
	};

	struct FHabitatSpotLightCache final
	{
		float m_fRadius = 1.0f;								// 영향 범위
		float m_fCosAngle = 0.0f;							// 코사인 각 ( 영향 범위각 )
		Vector3 m_vEnergy = { 0.0f, 0.0f, 0.0f };			// 빛 에너지 총량
		Vector3 m_vPosition = { 0.0f, 0.0f, 0.0f };			// 빛 시작점
		Vector3 m_vDirection = { 0.0f, 0.0f, 0.0f };		// 방향
		std::tuple<int32, int32, int32> m_tpOctreeNodeIndex = { 0, 0, 0 };
	};

	struct FHabitatPointLightCache final
	{
		float m_fRadius = 1.0f;							// 영향 범위 반지름
		Vector3 m_vEnergy = { 0.0f, 0.0f, 0.0f };			// 빛 에너지 총량
		Vector3 m_vPosition = { 0.0f, 0.0f, 0.0f };			// 중점 (발광점)
		std::tuple<int32, int32, int32> m_tpOctreeNodeIndex = { 0, 0, 0 };
	};

	struct FHabitatDirectionalLightCache final
	{
		float m_fSolidAngle = 0.0f;					// 진하게 보이는 각
		float m_fPerceivedRadius = 1.0f;					// 빛 작동 범위
		float m_fPerceivedDistance = 1.0f;					// 빛 작동 거리
		Vector3 m_vEnergy = { 0.0f, 0.0f, 0.0f };	// 빛 에너지 총량
		Vector3 m_vDirection = { 0.0f, 0.0f, 0.0f };	// 방향
		std::tuple<int32, int32, int32> m_tpOctreeNodeIndex = { 0, 0, 0 };
	};

	struct FHabitatCameraFrustumCache final
	{
		float m_fRightSlope = 1.0f;
		float m_fLeftSlope = 1.0f;
		float m_fTopSlope = 1.0f;
		float m_fBottomSlope = 1.0f;
		float m_fNearPlane = 0.0f;
		float m_fFarPlane = 1.0f;
		Vector3 m_vCenter = { 0.0f, 0.0f, 0.0f };			// 중점 입니다.
		Vector3 m_vRadian = { 0.0f, 0.0f, 0.0f };			// 라디안값 입니다.
	};

	/********************
		Produce/Consume
	********************/
	struct FProduceStatus
	{
		// 생산 중인 아이템 타입
		EItemType m_eItemType = EItemType::ENUM_END;

		// 총 생산할 개수
		int32 m_iNumItems = 0;

		// 생산된 개수
		int32 m_iNumProducedItem = 0;

		// 한 번에 생산할 수 있는 아이템 개수
		int32 m_iNumItemAtOnce = 0;

		// 한 아이템을 생산하는 데 걸리는 시간
		float m_fMaxDeltaSeconds = 0.0f;

		// 한 아이템을 생산한 누적된 시간
		float m_fSumDeltaSeconds = 0.0f;

		// 생산 진행률
		float m_iCurPercentage = 0.0f;

		// 생산 완료 여부
		EProduceItemState m_eProduceCompleted = EProduceItemState::NOT_COMPLETED;
	};

	struct FBuildStatus
	{
		// 건축 중인 건물 타입
		EArchitectureType m_eArchitectureType = EArchitectureType::ENUM_END;

		// 건축 가능 반경
		float m_fBuildEnableRadius = 0.0f;

		// 건축 소요 시간
		float m_fMaxDeltaSeconds = 0.0f;

		// 건축 누적 시간
		float m_fSumDeltaSeconds = 0.0f;

		// 건축 진행률
		float m_iCurPercentage = 0.0f;

		// 건축 완료 여부
		bool m_bBuildCompleted = false;
	};

	/********************
		Container
	********************/
	struct FContainerInfo
	{
		int32 m_iIndexX = 0;
		int32 m_iIndexY = 0;
		std::wstring m_wstrTextureFileName = L"";
	};

	struct FInventoryItemInfo : public FContainerInfo
	{
		EItemType m_eItemType = EItemType::ENUM_END;
		EItemKindType m_eItemKindType = EItemKindType::ENUM_END;
		EWeaponType m_eWeaponType = EWeaponType::ENUM_END;
		EArmorType m_eArmorType = EArmorType::ENUM_END;
		EMineralType m_eMineralType = EMineralType::ENUM_END;
		EIngredientType m_eIngredientType = EIngredientType::ENUM_END;

		int32 m_iNumItem = 0;
		int32 m_iNutrition = 0;
		float m_fWeight = 0.0f;
		float m_fAttackPower = 0.0f;
		float m_fDefensePower = 0.0f;
		float m_fShieldCoolTime = 0.0f;
		float m_fDurability = 0.0f;
	};

	struct FPalBoxPalInfo : public FContainerInfo
	{
		EMonsterType m_eMonsterType							= EMonsterType::ENUM_END;
		EMonsterAIType m_eMonsterAIType						= EMonsterAIType::ENUM_END;
		EMonsterPersonalityType m_eMonsterPersonalityType	= EMonsterPersonalityType::PEACEFUL;
		EMonsterElementType m_eMonsterElementType			= EMonsterElementType::ENUM_END;

		wstring m_wstrMonsterName							= L"";
		int32 m_iMonsterLevel								= 0;
		float m_fCurHp										= 100.0f;
		float m_fCurHungerGauge								= 100.0f;
		float m_fCurExp										= 0.0f;

		EToDoType m_eToDoType								= EToDoType::ENUM_END; // palwork curmission
		float m_fAttackPower								= 1.0f;
		float m_fDefensePower								= 0.0f;
		float m_fWorkSpeed									= 1.0f;
		int32 m_iFoodAmount									= 0;
		int32 m_iPartnerSkillLevel							= 0;
		int32 m_iActiveSkillLevelA							= 0;
		int32 m_iActiveSkillLevelB							= 0;
		int32 m_iActiveSkillLevelC							= 0;
		wstring m_wstrPartnerSkillName						= L"";
		wstring m_wstrActiveSkillNameA						= L"";
		wstring m_wstrActiveSkillNameB						= L"";
		wstring m_wstrActiveSkillNameC						= L"";
		EMonsterElementType m_eActiveSkillElementA			= EMonsterElementType::ENUM_END;
		EMonsterElementType m_eActiveSkillElementB			= EMonsterElementType::ENUM_END;
		EMonsterElementType m_eActiveSkillElementC			= EMonsterElementType::ENUM_END;
		wstring m_wstrPassiveSkillNameA						= L"";
		wstring m_wstrPassiveSkillNameB						= L"";
	};

	/********************
		Customize
	********************/
	struct FCustomObjectData
	{
		std::wstring m_wstrHeadMeshPath = L"";
		std::wstring m_wstrHairMeshPath = L"";
		EItemType m_eInitialHeadEquip = EItemType::NONE;
		EItemType m_eInitialOutfit = EItemType::NONE;

		int32 m_iCorneaIndex = 0;
		int32 m_iHighlightIndex = 0;

		Vector3 m_vColor = Vector3(0.f, 0.f, 0.f);
	};
}