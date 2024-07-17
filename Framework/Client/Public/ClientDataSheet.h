#pragma once

/********************
	인게임 상수
********************/
namespace Client
{
	struct FClientDataSheet
	{
	public:
		void Release()
		{
			k_mapKnockBackValue.clear();
			k_mapPalPossibleDrops.clear();
			k_mapPalDefaultAttackPowers.clear();
			k_mapPalDefaultDefensePowers.clear();
			k_mapPalDefaultRunSpeeds.clear();
			k_mapPalDefaultMoveSpeeds.clear();
			k_mapPalDefaultCreepSpeeds.clear();
			k_mapPalHPs.clear();
			k_mapPalDefaultToDoRatios.clear();
			k_mapPalHungerToDoRatios.clear();
			k_mapPalFoodAmounts.clear();
			k_mapPalPartnerSkillTypes.clear();
			k_mapPalActionTimeTypes.clear();
			k_mapPalElementTypes.clear();
			k_mapPalTaskTypes.clear();
			k_mapPalPartnerSkillName.clear();
			k_mapPalActiveSkillNameA.clear();
			k_mapPalActiveSkillNameB.clear();
			k_mapPalActiveSkillNameC.clear();
			k_mapPalPassiveSkillNameA.clear();
			k_mapPalPassiveSkillNameB.clear();
			k_mapPalName.clear();
			k_mapItemDefaultWeight.clear();
			k_mapWeaponDefaultAttackPowers.clear();
			k_mapArmorDefaultDefensePowers.clear();
			k_mapIngredientDefaultNutrition.clear();
			k_mapItemProduceStatuses.clear();

			//
			k_vecBakedBerriesSheet.clear();
			k_vecSheepballKebabSheet.clear();
			k_vecChickenKebabSheet.clear();
			k_vecFriedEggSheet.clear();
			k_vecSheepballRoastSheet.clear();
			k_vecChickenRoastSheet.clear();

			k_vecCopperIngotSheet.clear();
			k_vecIronIngotSheet.clear();
			k_vecGunpowderSheet.clear();
			k_vecCorbonSheet.clear();

			k_vecPalSphereBlueSheet.clear();
			k_vecPalSphereGreenSheet.clear();
			k_vecGliderSheet.clear();
			k_vecArrowSheet.clear();
			k_vecClothArmorSheet.clear();
			k_vecFurArmorSheet.clear();
			k_vecIronArmorColdSheet.clear();
			k_vecRifleBulletSheet.clear();
			k_vecBirdHeadSheet.clear();
			k_vecPenguinHeadSheet.clear();
			k_vecCopperHelmetSheet.clear();
			k_vecStealHelmetSheet.clear();

			k_vecHandTorchSheet.clear();
			k_vecStoneSpearSheet.clear();
			k_vecMeatCleaverSheet.clear();
			k_vecOldBowSheet.clear();
			k_vecRocketLauncherSheet.clear();
			k_vecStoneAxeSheet.clear();
			k_vecStonePickAxeSheet.clear();
			k_vecAssaultRifleSheet.clear();
			k_vecCommonShieldSheet.clear();
			k_vecGigaShieldSheet.clear();
		}

	public:
		const float k_fPlayerInteractRadius = 1.5f;

		// 최대 스태미나 게이지
		const float k_fMaxStaminaGauge = 100.0f;

		// 최대 공복 수치
		const float k_fMaxHungerGauge = 100.0f;

		// 최대 파트너 스킬 게이지
		const float k_fMaxPartnerSkillGauge = 100.0f;

		// 팰 최대 레벨
		const int k_iMaxCreatureLevel = 50;

		// 초당 허기 게이지 증가량
		const float k_fIncreaseHungerGaugePerSeconds = 10.0f;

		// 초당 허기 게이지 감소량
		const float k_fDecreaseHungerGaugePerSeconds = 0.05f;

		// 나무, 바위가 리스폰되는데 걸리는 최대 시간
		const float k_fMaxNatureRespawnDeltaSeconds = 120.0f;

		// 나무, 바위에서 아이템이 드롭될 확률
		const float k_fItemDropPercentage = 0.4f;

		// 나무 최대 체력
		const float k_fMaxTreeHp = 100.0f;

		// 바위 최대 체력
		const float k_fMaxRockHp = 100.0f;

		// 나무, 바위 최대 흔들림 시간
		const float k_fMaxShakeDeltaSeconds = 0.2f;

		// 거점 팰의 바위 채굴 대미지
		const float k_fMineDamage = 10.0f;

		// 거점 팰의 나무 베기 대미지
		const float k_fCutDownDamage = 10.0f;

		// 바위를 한 번 캐는데 걸리는 시간
		const float k_fMaxMineDeltaSeconds = 3.0f;

		// 나무를 한 번 베는데 걸리는 시간
		const float k_fMaxCutDownDeltaSeconds = 3.0f;

		// 넉백
		std::map<EMonsterType, float> k_mapKnockBackValue
		{
			{ EMonsterType::BOAR,			15.0f },
			{ EMonsterType::GRASS_MAMMOTH,	20.0f },
			{ EMonsterType::PINK_CAT,		10.0f },
			{ EMonsterType::SHEEP_BALL,		10.0f },
		};

		// 드롭
		std::map<EMonsterType, std::vector<EItemType>> k_mapPalPossibleDrops
		{
			{ EMonsterType::BERRY_GOAT,		{ EItemType::RED_BERRY } },
			{ EMonsterType::BOAR,			{ EItemType::BOAR_MEAT, EItemType::LEATHER } },
			{ EMonsterType::CARBUNCLO,		{ } },// x
			{ EMonsterType::CHICKEN_PAL,	{ } },// x
			{ EMonsterType::DREAM_DEMON,	{ } },// x
			{ EMonsterType::ELEC_PANDA,		{ } },// x
			{ EMonsterType::GRASS_MAMMOTH,	{ EItemType::GRASSMAMMOTH_MEAT } },
			{ EMonsterType::HEDGE_HOG,		{ EItemType::IRON } },
			{ EMonsterType::HEDGE_HOG_ICE,	{ } },
			{ EMonsterType::KITSUNEBI,		{ } },
			{ EMonsterType::PENGUIN,		{ } },
			{ EMonsterType::PINK_CAT,		{ EItemType::WOOL } },
			{ EMonsterType::SHEEP_BALL,		{ EItemType::WOOL, EItemType::SHEEPBALL_MEAT } },
			{ EMonsterType::WOOL_FOX,		{ EItemType::WOOL, EItemType::EGG } },
		};

		// 체력
		std::map<EMonsterType, float> k_mapPalHPs
		{
			{ EMonsterType::BERRY_GOAT,		100.0f },
			{ EMonsterType::BOAR,			80.0f },
			{ EMonsterType::CARBUNCLO,		75.0f },
			{ EMonsterType::CHICKEN_PAL,	60.0f },
			{ EMonsterType::DREAM_DEMON,	70.0f },
			{ EMonsterType::ELEC_PANDA,		3333.0f },
			{ EMonsterType::GRASS_MAMMOTH,	777.0f },
			{ EMonsterType::HEDGE_HOG,		70.0f },
			{ EMonsterType::HEDGE_HOG_ICE,	70.0f },
			{ EMonsterType::KITSUNEBI,		65.0f },
			{ EMonsterType::PENGUIN,		70.0f },
			{ EMonsterType::PINK_CAT,		70.0f },
			{ EMonsterType::SHEEP_BALL,		70.0f },
			{ EMonsterType::WOOL_FOX,		70.0f },
		};

		// 공격력
		std::map<EMonsterType, float> k_mapPalDefaultAttackPowers
		{
			{ EMonsterType::BERRY_GOAT,		70.0f },
			{ EMonsterType::BOAR,			80.0f },
			{ EMonsterType::CARBUNCLO,		70.0f },
			{ EMonsterType::CHICKEN_PAL,	60.0f },
			{ EMonsterType::DREAM_DEMON,	75.0f },
			{ EMonsterType::ELEC_PANDA,		120.0f },
			{ EMonsterType::GRASS_MAMMOTH,	120.0f },
			{ EMonsterType::HEDGE_HOG,		75.0f },
			{ EMonsterType::HEDGE_HOG_ICE,	75.0f },
			{ EMonsterType::KITSUNEBI,		75.0f },
			{ EMonsterType::PENGUIN,		75.0f },
			{ EMonsterType::PINK_CAT,		70.0f },
			{ EMonsterType::SHEEP_BALL,		70.0f },
			{ EMonsterType::WOOL_FOX,		70.0f },
		};

		// 방어력
		std::map<EMonsterType, float> k_mapPalDefaultDefensePowers
		{
			{ EMonsterType::BERRY_GOAT,		80.0f },
			{ EMonsterType::BOAR,			70.0f },
			{ EMonsterType::CARBUNCLO,		70.0f },
			{ EMonsterType::CHICKEN_PAL,	60.0f },
			{ EMonsterType::DREAM_DEMON,	60.0f },
			{ EMonsterType::ELEC_PANDA,		90.0f },
			{ EMonsterType::GRASS_MAMMOTH,	90.0f },
			{ EMonsterType::HEDGE_HOG,		70.0f },
			{ EMonsterType::HEDGE_HOG_ICE,	80.0f },
			{ EMonsterType::KITSUNEBI,		70.0f },
			{ EMonsterType::PENGUIN,		70.0f },
			{ EMonsterType::PINK_CAT,		70.0f },
			{ EMonsterType::SHEEP_BALL,		70.0f },
			{ EMonsterType::WOOL_FOX,		75.0f },
		};

		// 빠른 이동속도
		std::map<EMonsterType, float> k_mapPalDefaultRunSpeeds
		{
			{ EMonsterType::BERRY_GOAT,		6.0f },
			{ EMonsterType::BOAR,			8.0f },
			{ EMonsterType::CARBUNCLO,		6.0f },
			{ EMonsterType::CHICKEN_PAL,	6.0f },
			{ EMonsterType::DREAM_DEMON,	6.0f },
			{ EMonsterType::HEDGE_HOG,		6.0f },
			{ EMonsterType::HEDGE_HOG_ICE,	6.0f },
			{ EMonsterType::KITSUNEBI,		6.0f },
			{ EMonsterType::ELEC_PANDA,		10.0f },
			{ EMonsterType::GRASS_MAMMOTH,	6.0f },
			{ EMonsterType::PENGUIN,		6.0f },
			{ EMonsterType::PINK_CAT,		6.0f },
			{ EMonsterType::SHEEP_BALL,		6.0f },
			{ EMonsterType::WOOL_FOX,		6.0f },
		};

		// 기본 이동 속도
		std::map<EMonsterType, float> k_mapPalDefaultMoveSpeeds
		{
			{ EMonsterType::BERRY_GOAT,		3.0f },
			{ EMonsterType::BOAR,			3.0f },
			{ EMonsterType::CARBUNCLO,		3.0f },
			{ EMonsterType::CHICKEN_PAL,	3.0f },
			{ EMonsterType::DREAM_DEMON,	3.0f },
			{ EMonsterType::ELEC_PANDA,		3.0f },
			{ EMonsterType::GRASS_MAMMOTH,	3.0f },
			{ EMonsterType::HEDGE_HOG,		3.0f },
			{ EMonsterType::HEDGE_HOG_ICE,	3.0f },
			{ EMonsterType::KITSUNEBI,		3.0f },
			{ EMonsterType::PENGUIN,		3.0f },
			{ EMonsterType::PINK_CAT,		3.0f },
			{ EMonsterType::SHEEP_BALL,		3.0f },
			{ EMonsterType::WOOL_FOX,		3.0f },
		};
		
		// 느린 이동 속도
		std::map<EMonsterType, float> k_mapPalDefaultCreepSpeeds
		{
			{ EMonsterType::BERRY_GOAT,		1.2f },
			{ EMonsterType::BOAR,			1.2f },
			{ EMonsterType::CARBUNCLO,		1.2f },
			{ EMonsterType::CHICKEN_PAL,	1.2f },
			{ EMonsterType::DREAM_DEMON,	1.2f },
			{ EMonsterType::ELEC_PANDA,		1.2f },
			{ EMonsterType::GRASS_MAMMOTH,	1.2f },
			{ EMonsterType::HEDGE_HOG,		1.2f },
			{ EMonsterType::HEDGE_HOG_ICE,	1.2f },
			{ EMonsterType::KITSUNEBI,		1.2f },
			{ EMonsterType::PENGUIN,		1.2f },
			{ EMonsterType::PINK_CAT,		1.2f },
			{ EMonsterType::SHEEP_BALL,		1.2f },
			{ EMonsterType::WOOL_FOX,		1.2f },
		};

		// 작업 속도(100 : 1)
		std::map<EMonsterType, float> k_mapPalDefaultToDoRatios
		{
			{ EMonsterType::BERRY_GOAT,		1.0f },
			{ EMonsterType::BOAR,			1.0f },
			{ EMonsterType::CARBUNCLO,		1.0f },
			{ EMonsterType::CHICKEN_PAL,	1.0f },
			{ EMonsterType::DREAM_DEMON,	1.0f },
			{ EMonsterType::ELEC_PANDA,		1.0f },
			{ EMonsterType::GRASS_MAMMOTH,	1.0f },
			{ EMonsterType::HEDGE_HOG,		1.0f },
			{ EMonsterType::HEDGE_HOG_ICE,	1.0f },
			{ EMonsterType::KITSUNEBI,		1.0f },
			{ EMonsterType::PENGUIN,		1.0f },
			{ EMonsterType::PINK_CAT,		1.0f },
			{ EMonsterType::SHEEP_BALL,		1.0f },
			{ EMonsterType::WOOL_FOX,		1.0f },
		};

		std::map<EMonsterType, float> k_mapPalHungerToDoRatios
		{
			{ EMonsterType::BERRY_GOAT,		0.1f },
			{ EMonsterType::BOAR,			0.1f },
			{ EMonsterType::CARBUNCLO,		0.1f },
			{ EMonsterType::CHICKEN_PAL,	0.1f },
			{ EMonsterType::DREAM_DEMON,	0.1f },
			{ EMonsterType::ELEC_PANDA,		0.1f },
			{ EMonsterType::GRASS_MAMMOTH,	0.1f },
			{ EMonsterType::HEDGE_HOG,		0.1f },
			{ EMonsterType::HEDGE_HOG_ICE,	0.1f },
			{ EMonsterType::KITSUNEBI,		0.1f },
			{ EMonsterType::PENGUIN,		0.1f },
			{ EMonsterType::PINK_CAT,		0.1f },
			{ EMonsterType::SHEEP_BALL,		0.1f },
			{ EMonsterType::WOOL_FOX,		0.1f },
		};

		// 식사량
		std::map<EMonsterType, int32> k_mapPalFoodAmounts
		{
			{ EMonsterType::BERRY_GOAT,		4 },
			{ EMonsterType::BOAR,			3 },
			{ EMonsterType::CARBUNCLO,		2 },
			{ EMonsterType::CHICKEN_PAL,	2 },
			{ EMonsterType::DREAM_DEMON,	2 },
			{ EMonsterType::ELEC_PANDA,		8 },
			{ EMonsterType::GRASS_MAMMOTH,	8 },
			{ EMonsterType::HEDGE_HOG,		2 },
			{ EMonsterType::HEDGE_HOG_ICE,	2 },
			{ EMonsterType::KITSUNEBI,		2 },
			{ EMonsterType::PENGUIN,		2 },
			{ EMonsterType::PINK_CAT,		2 },
			{ EMonsterType::SHEEP_BALL,		2 },
			{ EMonsterType::WOOL_FOX,		2 },
		};

		std::map<EMonsterType, EPartnerSkillType> k_mapPalPartnerSkillTypes
		{
			{ EMonsterType::BERRY_GOAT,		EPartnerSkillType::BERRY_PICKER		},
			{ EMonsterType::BOAR,			EPartnerSkillType::HARD_HEAD },
			{ EMonsterType::CARBUNCLO,		EPartnerSkillType::LIFMUNK_RECOIL	},
			{ EMonsterType::CHICKEN_PAL,	EPartnerSkillType::EGG_LAYER		},
			{ EMonsterType::DREAM_DEMON,	EPartnerSkillType::DREAM_CHASER		},
			{ EMonsterType::ELEC_PANDA,		EPartnerSkillType::YELLOW_TANK		},
			{ EMonsterType::GRASS_MAMMOTH,	EPartnerSkillType::GAIA_CRUSHER		},
			{ EMonsterType::HEDGE_HOG,		EPartnerSkillType::JOLT_BOMB		},
			{ EMonsterType::HEDGE_HOG_ICE,	EPartnerSkillType::COLD_BOMB		},
			{ EMonsterType::KITSUNEBI,		EPartnerSkillType::HUGGY_FIRE		},
			{ EMonsterType::PENGUIN,		EPartnerSkillType::PENGULLET_CANNON },
			{ EMonsterType::PINK_CAT,		EPartnerSkillType::CAT_HELPER		},
			{ EMonsterType::SHEEP_BALL,		EPartnerSkillType::FLUFFY_SHIELD	},
			{ EMonsterType::WOOL_FOX,		EPartnerSkillType::FLUFFY_WOOL		},
		};

		std::map<EMonsterType, EMonsterActionTimeType> k_mapPalActionTimeTypes
		{
			{ EMonsterType::BERRY_GOAT,		EMonsterActionTimeType::DAY },
			{ EMonsterType::BOAR,			EMonsterActionTimeType::DAY },
			{ EMonsterType::CARBUNCLO,		EMonsterActionTimeType::DAY },
			{ EMonsterType::CHICKEN_PAL,	EMonsterActionTimeType::DAY },
			{ EMonsterType::DREAM_DEMON,	EMonsterActionTimeType::NIGHT },
			{ EMonsterType::ELEC_PANDA,		EMonsterActionTimeType::DAY },
			{ EMonsterType::GRASS_MAMMOTH,	EMonsterActionTimeType::DAY },
			{ EMonsterType::HEDGE_HOG,		EMonsterActionTimeType::DAY },
			{ EMonsterType::HEDGE_HOG_ICE,	EMonsterActionTimeType::DAY },
			{ EMonsterType::KITSUNEBI,		EMonsterActionTimeType::DAY },
			{ EMonsterType::PENGUIN,		EMonsterActionTimeType::DAY },
			{ EMonsterType::PINK_CAT,		EMonsterActionTimeType::DAY },
			{ EMonsterType::SHEEP_BALL,		EMonsterActionTimeType::DAY },
			{ EMonsterType::WOOL_FOX,		EMonsterActionTimeType::DAY },
		};

		std::map<EMonsterType, EMonsterElementType> k_mapPalElementTypes
		{
			{ EMonsterType::BERRY_GOAT,		EMonsterElementType::GRASS },
			{ EMonsterType::BOAR,			EMonsterElementType::GROUND },
			{ EMonsterType::CARBUNCLO,		EMonsterElementType::GRASS },
			{ EMonsterType::CHICKEN_PAL,	EMonsterElementType::NEUTRAL },
			{ EMonsterType::DREAM_DEMON,	EMonsterElementType::DARK },
			{ EMonsterType::ELEC_PANDA,		EMonsterElementType::ELECTRIC },
			{ EMonsterType::GRASS_MAMMOTH,	EMonsterElementType::GRASS },
			{ EMonsterType::HEDGE_HOG,		EMonsterElementType::ELECTRIC },
			{ EMonsterType::HEDGE_HOG_ICE,	EMonsterElementType::ICE },
			{ EMonsterType::KITSUNEBI,		EMonsterElementType::FIRE },
			{ EMonsterType::PENGUIN,		EMonsterElementType::WATER },
			{ EMonsterType::PINK_CAT,		EMonsterElementType::NEUTRAL },
			{ EMonsterType::SHEEP_BALL,		EMonsterElementType::NEUTRAL },
			{ EMonsterType::WOOL_FOX,		EMonsterElementType::NEUTRAL },
		};

		std::map<EMonsterType, std::vector<EMonsterTaskType>> k_mapPalTaskTypes
		{
			{ EMonsterType::BERRY_GOAT,		{ /*EMonsterTaskType::FARM,*/			EMonsterTaskType::PLANT																		} },
			{ EMonsterType::BOAR,			{ EMonsterTaskType::MINE																											} },
			{ EMonsterType::CARBUNCLO,		{ EMonsterTaskType::HANDIWORK,			EMonsterTaskType::LUMBER,		EMonsterTaskType::PLANT,									} },
			{ EMonsterType::CHICKEN_PAL,	{ /*EMonsterTaskType::FARM,*/																										} },
			{ EMonsterType::DREAM_DEMON,	{ EMonsterTaskType::HANDIWORK,			EMonsterTaskType::TRANSPORT,																} },
			{ EMonsterType::ELEC_PANDA,		{ EMonsterTaskType::LUMBER,				EMonsterTaskType::PLANT,		EMonsterTaskType::MINE										} },
			{ EMonsterType::GRASS_MAMMOTH,	{ EMonsterTaskType::LUMBER,				EMonsterTaskType::PLANT,		EMonsterTaskType::MINE										} },
			{ EMonsterType::HEDGE_HOG,		{ EMonsterTaskType::ELECTRICITY																										} },
			{ EMonsterType::HEDGE_HOG_ICE,	{ EMonsterTaskType::COOL																											} },
			{ EMonsterType::KITSUNEBI,		{ EMonsterTaskType::KINDLE																											} },
			{ EMonsterType::PENGUIN,		{ EMonsterTaskType::HANDIWORK,			EMonsterTaskType::PLANT,		EMonsterTaskType::TRANSPORT,	EMonsterTaskType::COOL		} },
			{ EMonsterType::PINK_CAT,		{ EMonsterTaskType::HANDIWORK,			EMonsterTaskType::TRANSPORT,	EMonsterTaskType::LUMBER									} },// gather에서 lumber로 변경했습니다.
			{ EMonsterType::SHEEP_BALL,		{ EMonsterTaskType::HANDIWORK,			EMonsterTaskType::TRANSPORT,	/*EMonsterTaskType::FARM*/									} },
			{ EMonsterType::WOOL_FOX,		{ /*EMonsterTaskType::FARM,*/																										} },
		};

		std::map<EMonsterType, std::wstring> k_mapPalPartnerSkillName
		{
			{ EMonsterType::BERRY_GOAT,		L"열매 채집" },
			{ EMonsterType::BOAR,			L"돌대가리" },
			{ EMonsterType::CARBUNCLO,		L"큐룰리스 리코일" },
			{ EMonsterType::CHICKEN_PAL,	L"알 생산" },
			{ EMonsterType::DREAM_DEMON,	L"꿈빛 체이서" },
			{ EMonsterType::ELEC_PANDA,		L"노란 중전차" },
			{ EMonsterType::GRASS_MAMMOTH,	L"가이아 크래셔" },
			{ EMonsterType::HEDGE_HOG,		L"찌르르 폭탄" },
			{ EMonsterType::HEDGE_HOG_ICE,	L"딱딱 폭탄" },
			{ EMonsterType::KITSUNEBI,		L"포옹 파이어" },
			{ EMonsterType::PENGUIN,		L"펭키 발사기" },
			{ EMonsterType::PINK_CAT,		L"고양이 손 빌리기" },
			{ EMonsterType::SHEEP_BALL,		L"복슬복슬 방패" },
			{ EMonsterType::WOOL_FOX,		L"푹신푹신 양털" },
		};

		std::map<EMonsterType, std::wstring> k_mapPalActiveSkillNameA
		{
			{ EMonsterType::BERRY_GOAT,		L"바람의 칼날" },
			{ EMonsterType::BOAR,			L"멧돼지 돌진" },
			{ EMonsterType::CARBUNCLO,		L"바람의 칼날" },
			{ EMonsterType::CHICKEN_PAL,	L"치킨 태클" },
			{ EMonsterType::DREAM_DEMON,	L"암흑구" },
			{ EMonsterType::ELEC_PANDA,		L"전기 할퀴기" },
			{ EMonsterType::GRASS_MAMMOTH,	L"대지 강타" },
			{ EMonsterType::HEDGE_HOG,		L"전기 파장" },
			{ EMonsterType::HEDGE_HOG_ICE,	L"얼음 미사일" },
			{ EMonsterType::KITSUNEBI,		L"파이어 샷" },
			{ EMonsterType::PENGUIN,		L"얼음 미사일" },
			{ EMonsterType::PINK_CAT,		L"냥냥 펀치" },
			{ EMonsterType::SHEEP_BALL,		L"데굴데굴 솜사탕" },
			{ EMonsterType::WOOL_FOX,		L"스킬 없음" },
		};

		std::map<EMonsterType, EMonsterElementType> k_mapPalActiveSkillElementTypeA
		{
			{ EMonsterType::BERRY_GOAT,		EMonsterElementType::GRASS },
			{ EMonsterType::BOAR,			EMonsterElementType::GROUND },
			{ EMonsterType::CARBUNCLO,		EMonsterElementType::GRASS },
			{ EMonsterType::CHICKEN_PAL,	EMonsterElementType::NEUTRAL },
			{ EMonsterType::DREAM_DEMON,	EMonsterElementType::DARK },
			{ EMonsterType::ELEC_PANDA,		EMonsterElementType::ELECTRIC },
			{ EMonsterType::GRASS_MAMMOTH,	EMonsterElementType::GROUND },
			{ EMonsterType::HEDGE_HOG,		EMonsterElementType::ELECTRIC },
			{ EMonsterType::HEDGE_HOG_ICE,	EMonsterElementType::ICE },
			{ EMonsterType::KITSUNEBI,		EMonsterElementType::FIRE },
			{ EMonsterType::PENGUIN,		EMonsterElementType::ICE },
			{ EMonsterType::PINK_CAT,		EMonsterElementType::NEUTRAL },
			{ EMonsterType::SHEEP_BALL,		EMonsterElementType::NEUTRAL },
			{ EMonsterType::WOOL_FOX,		EMonsterElementType::ENUM_END },
		};

		std::map<EMonsterType, std::wstring> k_mapPalActiveSkillNameB
		{
			{ EMonsterType::BERRY_GOAT,		L"스킬 없음" },
			{ EMonsterType::BOAR,			L"스킬 없음" },
			{ EMonsterType::CARBUNCLO,		L"스킬 없음" },
			{ EMonsterType::CHICKEN_PAL,	L"스킬 없음" },
			{ EMonsterType::DREAM_DEMON,	L"스킬 없음" },
			{ EMonsterType::ELEC_PANDA,		L"스킬 없음" },
			{ EMonsterType::GRASS_MAMMOTH,	L"씨앗 기관총" },
			{ EMonsterType::HEDGE_HOG,		L"스킬 없음" },
			{ EMonsterType::HEDGE_HOG_ICE,	L"스킬 없음" },
			{ EMonsterType::KITSUNEBI,		L"스킬 없음" },
			{ EMonsterType::PENGUIN,		L"스킬 없음" },
			{ EMonsterType::PINK_CAT,		L"스킬 없음" },
			{ EMonsterType::SHEEP_BALL,		L"스킬 없음" },
			{ EMonsterType::WOOL_FOX,		L"스킬 없음" },
		};

		std::map<EMonsterType, EMonsterElementType> k_mapPalActiveSkillElementTypeB
		{
			{ EMonsterType::BERRY_GOAT,		EMonsterElementType::ENUM_END },
			{ EMonsterType::BOAR,			EMonsterElementType::ENUM_END },
			{ EMonsterType::CARBUNCLO,		EMonsterElementType::ENUM_END },
			{ EMonsterType::CHICKEN_PAL,	EMonsterElementType::ENUM_END },
			{ EMonsterType::DREAM_DEMON,	EMonsterElementType::ENUM_END },
			{ EMonsterType::ELEC_PANDA,		EMonsterElementType::ENUM_END },
			{ EMonsterType::GRASS_MAMMOTH,	EMonsterElementType::ENUM_END },
			{ EMonsterType::HEDGE_HOG,		EMonsterElementType::ENUM_END },
			{ EMonsterType::HEDGE_HOG_ICE,	EMonsterElementType::ENUM_END },
			{ EMonsterType::KITSUNEBI,		EMonsterElementType::ENUM_END },
			{ EMonsterType::PENGUIN,		EMonsterElementType::ENUM_END },
			{ EMonsterType::PINK_CAT,		EMonsterElementType::ENUM_END },
			{ EMonsterType::SHEEP_BALL,		EMonsterElementType::ENUM_END },
			{ EMonsterType::WOOL_FOX,		EMonsterElementType::ENUM_END },
		};

		std::map<EMonsterType, std::wstring> k_mapPalActiveSkillNameC
		{
			{ EMonsterType::BERRY_GOAT,		L"스킬 없음" },
			{ EMonsterType::BOAR,			L"스킬 없음" },
			{ EMonsterType::CARBUNCLO,		L"스킬 없음" },
			{ EMonsterType::CHICKEN_PAL,	L"스킬 없음" },
			{ EMonsterType::DREAM_DEMON,	L"스킬 없음" },
			{ EMonsterType::ELEC_PANDA,		L"스킬 없음" },
			{ EMonsterType::GRASS_MAMMOTH,	L"스킬 없음" },
			{ EMonsterType::HEDGE_HOG,		L"스킬 없음" },
			{ EMonsterType::HEDGE_HOG_ICE,	L"스킬 없음" },
			{ EMonsterType::KITSUNEBI,		L"스킬 없음" },
			{ EMonsterType::PENGUIN,		L"스킬 없음" },
			{ EMonsterType::PINK_CAT,		L"스킬 없음" },
			{ EMonsterType::SHEEP_BALL,		L"스킬 없음" },
			{ EMonsterType::WOOL_FOX,		L"스킬 없음" },
		};

		std::map<EMonsterType, EMonsterElementType> k_mapPalActiveSkillElementTypeC
		{
			{ EMonsterType::BERRY_GOAT,		EMonsterElementType::ENUM_END },
			{ EMonsterType::BOAR,			EMonsterElementType::ENUM_END },
			{ EMonsterType::CARBUNCLO,		EMonsterElementType::ENUM_END },
			{ EMonsterType::CHICKEN_PAL,	EMonsterElementType::ENUM_END },
			{ EMonsterType::DREAM_DEMON,	EMonsterElementType::ENUM_END },
			{ EMonsterType::ELEC_PANDA,		EMonsterElementType::ENUM_END },
			{ EMonsterType::GRASS_MAMMOTH,	EMonsterElementType::ENUM_END },
			{ EMonsterType::HEDGE_HOG,		EMonsterElementType::ENUM_END },
			{ EMonsterType::HEDGE_HOG_ICE,	EMonsterElementType::ENUM_END },
			{ EMonsterType::KITSUNEBI,		EMonsterElementType::ENUM_END },
			{ EMonsterType::PENGUIN,		EMonsterElementType::ENUM_END },
			{ EMonsterType::PINK_CAT,		EMonsterElementType::ENUM_END },
			{ EMonsterType::SHEEP_BALL,		EMonsterElementType::ENUM_END },
			{ EMonsterType::WOOL_FOX,		EMonsterElementType::ENUM_END },
		};

		// 패시브 스킬 A
		std::map<EMonsterType, std::wstring> k_mapPalPassiveSkillNameA
		{ 
			{ EMonsterType::BERRY_GOAT,		L"스킬 없음" },
			{ EMonsterType::BOAR,			L"스킬 없음" },
			{ EMonsterType::CARBUNCLO,		L"스킬 없음" },
			{ EMonsterType::CHICKEN_PAL,	L"스킬 없음" },
			{ EMonsterType::DREAM_DEMON,	L"스킬 없음" },
			{ EMonsterType::ELEC_PANDA,		L"스킬 없음" },
			{ EMonsterType::GRASS_MAMMOTH,	L"스킬 없음" },
			{ EMonsterType::HEDGE_HOG,		L"스킬 없음" },
			{ EMonsterType::HEDGE_HOG_ICE,	L"스킬 없음" },
			{ EMonsterType::KITSUNEBI,		L"스킬 없음" },
			{ EMonsterType::PENGUIN,		L"스킬 없음" },
			{ EMonsterType::PINK_CAT,		L"스킬 없음" },
			{ EMonsterType::SHEEP_BALL,		L"스킬 없음" },
			{ EMonsterType::WOOL_FOX,		L"스킬 없음" },
		};

		// 패시브 스킬 B
		std::map<EMonsterType, std::wstring> k_mapPalPassiveSkillNameB
		{
			{ EMonsterType::BERRY_GOAT,		L"스킬 없음" },
			{ EMonsterType::BOAR,			L"스킬 없음" },
			{ EMonsterType::CARBUNCLO,		L"스킬 없음" },
			{ EMonsterType::CHICKEN_PAL,	L"스킬 없음" },
			{ EMonsterType::DREAM_DEMON,	L"스킬 없음" },
			{ EMonsterType::ELEC_PANDA,		L"스킬 없음" },
			{ EMonsterType::GRASS_MAMMOTH,	L"스킬 없음" },
			{ EMonsterType::HEDGE_HOG,		L"스킬 없음" },
			{ EMonsterType::HEDGE_HOG_ICE,	L"스킬 없음" },
			{ EMonsterType::KITSUNEBI,		L"스킬 없음" },
			{ EMonsterType::PENGUIN,		L"스킬 없음" },
			{ EMonsterType::PINK_CAT,		L"스킬 없음" },
			{ EMonsterType::SHEEP_BALL,		L"스킬 없음" },
			{ EMonsterType::WOOL_FOX,		L"스킬 없음" },
		};

		// 팰 이름
		std::map<EMonsterType, std::wstring> k_mapPalName
		{
			{ EMonsterType::BERRY_GOAT,		L"베리고트" },
			{ EMonsterType::BOAR,			L"돌진 돼지" },
			{ EMonsterType::CARBUNCLO,		L"큐룰리스" },
			{ EMonsterType::CHICKEN_PAL,	L"꼬꼬닭" },
			{ EMonsterType::DREAM_DEMON,	L"몽마둥이" },
			{ EMonsterType::ELEC_PANDA,		L"일렉판다" },
			{ EMonsterType::GRASS_MAMMOTH,	L"그린모스" },
			{ EMonsterType::HEDGE_HOG,		L"찌릿도치" },
			{ EMonsterType::HEDGE_HOG_ICE,	L"코치도치" },
			{ EMonsterType::KITSUNEBI,		L"파이호" },
			{ EMonsterType::PENGUIN,		L"펭키" },
			{ EMonsterType::PINK_CAT,		L"까부냥" },
			{ EMonsterType::SHEEP_BALL,		L"도로롱" },
			{ EMonsterType::WOOL_FOX,		L"밀피" },
		};

		std::map<EItemType, float> k_mapItemDefaultWeight
		{
			{ EItemType::HAND_HELD_TORCH,		1.0f},
			{ EItemType::STONE_SPEAR,			1.0f},
			{ EItemType::MEAT_CLEAVER,			1.0f},
			{ EItemType::OLD_BOW,				1.0f},
			{ EItemType::ROCKET_LAUNCHER,		1.0f},
			{ EItemType::STONE_AXE,				1.0f},
			{ EItemType::STONE_PICKAXE,			1.0f},
			{ EItemType::ASSAULT_RIFLE,			1.0f},
			{ EItemType::CONSTRUCT_HAMMER,		1.0f},
			{ EItemType::ARROW,					1.0f},
			{ EItemType::BLUE_SPHERE,			1.0f},
			{ EItemType::GREEN_SPHERE,			1.0f},

			{ EItemType::Outfit_OldCloth001,	1.0f},
			{ EItemType::Outfit_Cloth001,		1.0f},
			{ EItemType::Outfit_Hunter001,		1.0f},
			{ EItemType::Outfit_Platinum001_v02,1.0f},
			{ EItemType::HeadEquip016,			1.0f},
			{ EItemType::HeadEquip018,			1.0f},
			{ EItemType::HeadEquip019,			1.0f},
			{ EItemType::HeadEquip021,			1.0f},
			{ EItemType::HeadEquip029,			1.0f},
			{ EItemType::HeadEquip033,			1.0f},
			{ EItemType::COMMON_SHIELD,			1.0f},
			{ EItemType::GIGA_SHIELD,			1.0f},
			{ EItemType::GLIDER,				1.0f},

			{ EItemType::PALDIUM_FRAGMENT,		1.0f},
			{ EItemType::GOLD_COIN,				1.0f},
			{ EItemType::STONE,					1.0f},
			{ EItemType::WOOD,					1.0f},
			{ EItemType::LEATHER,				1.0f},
			{ EItemType::COAL,					1.0f},
			{ EItemType::WOOL,					1.0f},
			{ EItemType::IRON,					1.0f},
			{ EItemType::CLOTH,					1.0f},
			{ EItemType::COPPER_INGOT,			1.0f},
			{ EItemType::IRON_INGOT,			1.0f},
			{ EItemType::GUNPOWDER,				1.0f},
			{ EItemType::CARBON,				1.0f},
			{ EItemType::RIFLE_BULLET,			1.0f},

			{ EItemType::EGG,					1.0f},
			{ EItemType::SHEEPBALL_MEAT,		1.0f},
			{ EItemType::BOAR_MEAT,				1.0f},
			{ EItemType::RED_BERRY,				1.0f},
			{ EItemType::BAKED_BERRY,			1.0f},
			{ EItemType::FRIED_EGG,				1.0f},
			{ EItemType::SHEEPBALL_KEBAB,		1.0f},
			{ EItemType::GRASSMAMMOTH_MEAT,		1.0f},
			{ EItemType::SHEEPBALL_ROAST,		1.0f},
			{ EItemType::CHICKEN_ROAST,			1.0f},
		};

		std::map<EItemType, float> k_mapWeaponDefaultAttackPowers
		{ 
			{ EItemType::HAND_HELD_TORCH,		2.0f },
			{ EItemType::STONE_SPEAR,			3.0f },
			{ EItemType::MEAT_CLEAVER,			4.0f },// x
			{ EItemType::OLD_BOW,				1.0f },// x
			{ EItemType::ROCKET_LAUNCHER,		6.0f },// x
			{ EItemType::STONE_AXE,				5.0f },
			{ EItemType::STONE_PICKAXE,			5.0f },
			{ EItemType::ASSAULT_RIFLE,			15.0f },
			{ EItemType::CONSTRUCT_HAMMER,		1.0f },// x
			{ EItemType::ARROW,					3.0f },
			{ EItemType::BLUE_SPHERE,			0.0f },
			{ EItemType::GREEN_SPHERE,			0.0f },
		};

		std::map<EItemType, float> k_mapArmorDefaultDefensePowers
		{
			{ EItemType::Outfit_OldCloth001,	  8.0f },
			{ EItemType::Outfit_Cloth001,		  8.0f },
			{ EItemType::Outfit_Hunter001,		  8.0f },
			{ EItemType::Outfit_Platinum001_v02,  11.0f },
			{ EItemType::HeadEquip016,			  4.0f },
			{ EItemType::HeadEquip018,			  4.0f },
			{ EItemType::HeadEquip019,			  4.0f },
			{ EItemType::HeadEquip021,			  4.0f },
			{ EItemType::HeadEquip029,			  4.0f },
			{ EItemType::HeadEquip033,			  4.0f },
			{ EItemType::COMMON_SHIELD,			  8.0f },
			{ EItemType::GIGA_SHIELD,			  10.0f },
			{ EItemType::GLIDER,				  0.0f },
		};

		std::map<EIngredientType, int32> k_mapIngredientDefaultNutrition
		{
			{ EIngredientType::EGG,					6 },
			{ EIngredientType::SHEEPBALL_MEAT,		20 },
			{ EIngredientType::BOAR_MEAT,			20 },
			{ EIngredientType::RED_BERRY,			10 },
			{ EIngredientType::BAKED_BERRY,			20 },
			{ EIngredientType::FRIED_EGG,			30 },
			{ EIngredientType::SHEEPBALL_KEBAB,		35 },
			{ EIngredientType::CHICKEN_KEBAB,		35 },
			{ EIngredientType::SHEEPBALL_ROAST,		50 },
			{ EIngredientType::CHICKEN_ROAST,		50 },
		};

		// 아이템 생산 시간, 생산량
		std::map<EItemType, std::pair<float, int32>> k_mapItemProduceStatuses
		{
			{ EItemType::STONE_SPEAR,				{ 10.0f, 1 } },
			{ EItemType::HAND_HELD_TORCH,			{ 10.0f, 1 } },
			{ EItemType::MEAT_CLEAVER,				{ 10.0f, 1 } },
			{ EItemType::ROCKET_LAUNCHER,			{ 10.0f, 1 } },
			{ EItemType::STONE_AXE,					{ 10.0f, 1 } },
			{ EItemType::STONE_PICKAXE,				{ 10.0f, 1 } },
			{ EItemType::ASSAULT_RIFLE,				{ 10.0f, 1 } },
			{ EItemType::CONSTRUCT_HAMMER,			{ 10.0f, 1 } },
			{ EItemType::ARROW,						{ 10.0f, 3 } },
			{ EItemType::BLUE_SPHERE,				{ 10.0f, 1 } },
			{ EItemType::GREEN_SPHERE,				{ 10.0f, 1 } },
			{ EItemType::FRYING_PAN,				{ 10.0f, 1 } },

			{ EItemType::Outfit_OldCloth001,		{ 10.0f, 1 } },// 기본 옷
			{ EItemType::Outfit_Cloth001,			{ 15.0f, 1 } },// 천 옷
			{ EItemType::Outfit_Hunter001,			{ 15.0f, 1 } },// 가죽 옷
			{ EItemType::Outfit_Platinum001_v02,	{ 10.0f, 1 } },// 최고 방어구-한랭
			{ EItemType::HeadEquip016,				{ 10.0f, 1 } },// 브론즈 모자
			{ EItemType::HeadEquip018,				{ 10.0f, 1 } },// 테크 모자
			{ EItemType::HeadEquip019,				{ 10.0f, 1 } },// 알록새 모자
			{ EItemType::HeadEquip021,				{ 10.0f, 1 } },// 펭킹 모자
			{ EItemType::HeadEquip029,				{ 10.0f, 1 } },// 도로롱 모자
			{ EItemType::HeadEquip033,				{ 10.0f, 1 } },// 까부냥 모자
			{ EItemType::COMMON_SHIELD,				{ 10.0f, 1 } },// 기본 실드
			{ EItemType::GIGA_SHIELD,				{ 10.0f, 1 } },// 기가 실드
			{ EItemType::GLIDER,					{ 10.0f, 1 } },// 글라이더
			// { EItemType::BROAD_SWORD,				{ 10.0f, 1 } },// 검

			{ EItemType::PALDIUM_FRAGMENT,			{ 10.0f, 1 } },// 팰지움 파편
			{ EItemType::GOLD_COIN,					{ 10.0f, 1 } },// 금화
			{ EItemType::STONE,						{  3.0f, 2 } },// 돌
			{ EItemType::WOOD,						{  3.0f, 2 } },// 나무
			{ EItemType::LEATHER,					{ 10.0f, 1 } },// 가죽
			{ EItemType::COAL,						{ 10.0f, 1 } },// 석탄
			{ EItemType::WOOL,						{ 10.0f, 1 } },// 양털
			{ EItemType::IRON,						{ 10.0f, 1 } },// 철
			{ EItemType::CLOTH,						{ 10.0f, 1 } },// 천
			{ EItemType::COPPER_INGOT,				{  5.0f, 1 } },// 금속 주괴
			{ EItemType::IRON_INGOT,				{  5.0f, 1 } },// 제련 주괴
			{ EItemType::GUNPOWDER,					{  5.0f, 1 } },// 화약
			{ EItemType::CARBON,					{ 10.0f, 1 } },// 카본 섬유
			{ EItemType::RIFLE_BULLET,				{ 10.0f, 3 } },// 탄약

			// { EItemType::EGG,					{ 10.0f, 1 } },// 알
			// { EItemType::SHEEPBALL_MEAT,			{ 10.0f, 1 } },// 도로롱
			// { EItemType::BOAR_MEAT,				{ 10.0f, 1 } },// 꼬꼬닭

			{ EItemType::RED_BERRY,					{  3.0f, 2 } },// 빨간 열매
			{ EItemType::BAKED_BERRY,				{  5.0f, 1 } },// 구운 열매
			{ EItemType::FRIED_EGG,					{  5.0f, 1 } },// 계란 후라이
			{ EItemType::SHEEPBALL_KEBAB,			{  8.0f, 1 } },// 도로롱 꼬치
			{ EItemType::GRASSMAMMOTH_MEAT,				{  8.0f, 1 } },// 꼬꼬닭 꼬치
			{ EItemType::SHEEPBALL_ROAST,			{  8.0f, 1 } },// 도로롱 구이
			{ EItemType::CHICKEN_ROAST,				{  8.0f, 1 } },// 꼬꼬닭 구이
		};

		std::map<EMonsterType, std::wstring > k_mapPalIconTextureNames
		{
			{ EMonsterType::BERRY_GOAT,		L"T_BerryGoat_icon_normal" },
			{ EMonsterType::BOAR,			L"T_Boar_icon_normal" },
			{ EMonsterType::CARBUNCLO,		L"T_Carbunclo_icon_normal" },
			{ EMonsterType::CHICKEN_PAL,	L"T_ChickenPal_icon_normal" },
			{ EMonsterType::DREAM_DEMON,	L"T_DreamDemon_icon_normal" },
			{ EMonsterType::ELEC_PANDA,		L"T_ElecPanda_icon_normal" },
			{ EMonsterType::HEDGE_HOG,		L"T_Hedgehog_icon_normal" },
			{ EMonsterType::HEDGE_HOG_ICE,	L"T_HedgehogIce_icon_normal" },
			{ EMonsterType::KITSUNEBI,		L"T_Kitsunebi_icon_normal" },
			{ EMonsterType::PINK_CAT,		L"T_PinkCat_icon_normal" },
			{ EMonsterType::PENGUIN,		L"T_Penguin_icon_normal" },
			{ EMonsterType::SHEEP_BALL,		L"T_SheepBall_icon_normal" },
			{ EMonsterType::WOOL_FOX,		L"T_WoolFox_icon_normal" },
		};
		
		/******************
			데이터시트
		******************/
		// 요리
		// 구운 열매
		vector<pair<EItemType, int32>> k_vecBakedBerriesSheet
		{ pair(EItemType::RED_BERRY, 1) };
		// 도로롱 꼬치
		vector<pair<EItemType, int32>> k_vecSheepballKebabSheet
		{ pair(EItemType::SHEEPBALL_MEAT, 1) };
		// 꼬꼬닭 꼬치
		vector<pair<EItemType, int32>> k_vecChickenKebabSheet
		{ pair(EItemType::BOAR_MEAT, 1) };
		// 계란후라이
		vector<pair<EItemType, int32>> k_vecFriedEggSheet
		{ pair(EItemType::EGG, 1) };
		// 도로롱 구이
		vector<pair<EItemType, int32>> k_vecSheepballRoastSheet
		{ pair(EItemType::SHEEPBALL_MEAT, 1), pair(EItemType::RED_BERRY, 1) };
		// 꼬꼬닭 구이
		vector<pair<EItemType, int32>> k_vecChickenRoastSheet
		{ pair(EItemType::BOAR_MEAT, 1), pair(EItemType::RED_BERRY, 1) };

		// 화로
		// 금속 주괴
		vector<pair<EItemType, int32>> k_vecCopperIngotSheet
		{ pair(EItemType::IRON, 2) };
		// 제련 주괴
		vector<pair<EItemType, int32>> k_vecIronIngotSheet
		{ pair(EItemType::IRON, 2), pair(EItemType::COAL, 2) };
		// 화약
		vector<pair<EItemType, int32>> k_vecGunpowderSheet
		{ pair(EItemType::WOOD, 3) };
		// 카본 섬유
		vector<pair<EItemType, int32>> k_vecCorbonSheet
		{ pair(EItemType::COAL, 2) };

		// 일반
		// 팰 스피어(일반)
		vector<pair<EItemType, int32>> k_vecPalSphereBlueSheet
		{ pair(EItemType::WOOD, 3), pair(EItemType::STONE, 3), pair(EItemType::PALDIUM_FRAGMENT, 1) };
		// 팰 스피어(메가)
		vector<pair<EItemType, int32>> k_vecPalSphereGreenSheet
		{ pair(EItemType::WOOD, 3), pair(EItemType::STONE, 3), pair(EItemType::PALDIUM_FRAGMENT, 1), pair(EItemType::COPPER_INGOT, 1) };
		// 글라이더
		vector<pair<EItemType, int32>> k_vecGliderSheet
		{ pair(EItemType::WOOD, 5), pair(EItemType::WOOL, 2) };
		// 화살
		vector<pair<EItemType, int32>> k_vecArrowSheet
		{ pair(EItemType::WOOD, 1), pair(EItemType::STONE, 1) };
		// 천 옷
		vector<pair<EItemType, int32>> k_vecClothArmorSheet
		{ pair(EItemType::WOOL, 2) };
		// 가죽 갑옷
		vector<pair<EItemType, int32>> k_vecFurArmorSheet
		{ pair(EItemType::LEATHER, 5) };
		// 팰 금속 갑옷
		vector<pair<EItemType, int32>> k_vecIronArmorColdSheet
		{ pair(EItemType::LEATHER, 6), pair(EItemType::WOOL, 5), pair(EItemType::IRON_INGOT, 8) };
		// 탄약
		vector<pair<EItemType, int32>> k_vecRifleBulletSheet
		{ pair(EItemType::GUNPOWDER, 2) };
		// 알록새 모자
		vector<pair<EItemType, int32>> k_vecBirdHeadSheet
		{ pair(EItemType::WOOL, 4) };
		// 펭킹 모자
		vector<pair<EItemType, int32>> k_vecPenguinHeadSheet
		{ pair(EItemType::WOOL, 2), pair(EItemType::WOOL, 2) };
		// 금속 투구
		vector<pair<EItemType, int32>> k_vecCopperHelmetSheet
		{ pair(EItemType::PALDIUM_FRAGMENT, 2), pair(EItemType::COPPER_INGOT, 2) };
		// 팰 금속 투구
		vector<pair<EItemType, int32>> k_vecStealHelmetSheet
		{ pair(EItemType::PALDIUM_FRAGMENT, 5), pair(EItemType::IRON_INGOT, 3) };

		// 장비
		// 휴대형 횃불
		vector<pair<EItemType, int32>> k_vecHandTorchSheet
		{ pair(EItemType::WOOD, 1), pair(EItemType::STONE, 1) };
		// 돌 창
		vector<pair<EItemType, int32>> k_vecStoneSpearSheet
		{ pair(EItemType::WOOD, 8), pair(EItemType::STONE, 4) };
		// 푸주간 칼
		vector<pair<EItemType, int32>> k_vecMeatCleaverSheet
		{ pair(EItemType::WOOD, 5), pair(EItemType::STONE, 3), pair(EItemType::COPPER_INGOT, 1) };
		// 오래된 활
		vector<pair<EItemType, int32>> k_vecOldBowSheet
		{ pair(EItemType::WOOD, 10), pair(EItemType::STONE, 3), pair(EItemType::WOOL, 2) };
		// 로켓 런처(펭키의 로켓발사기)
		vector<pair<EItemType, int32>> k_vecRocketLauncherSheet
		{ pair(EItemType::WOOD, 5), pair(EItemType::STONE, 5), pair(EItemType::PALDIUM_FRAGMENT, 5), pair(EItemType::COPPER_INGOT, 5) };
		// 돌도끼
		vector<pair<EItemType, int32>> k_vecStoneAxeSheet
		{ pair(EItemType::WOOD, 4), pair(EItemType::STONE, 4) };
		// 돌곡괭이
		vector<pair<EItemType, int32>> k_vecStonePickAxeSheet
		{ pair(EItemType::WOOD, 4), pair(EItemType::STONE, 4) };
		// 돌격 소총
		vector<pair<EItemType, int32>> k_vecAssaultRifleSheet
		{ pair(EItemType::IRON_INGOT, 5), pair(EItemType::CARBON, 2) };
		// 일반 방패
		vector<pair<EItemType, int32>> k_vecCommonShieldSheet
		{ pair(EItemType::PALDIUM_FRAGMENT, 5), pair(EItemType::WOOD, 5), pair(EItemType::STONE, 5) };
		// 기가 방패
		vector<pair<EItemType, int32>> k_vecGigaShieldSheet
		{ pair(EItemType::PALDIUM_FRAGMENT, 10), pair(EItemType::COPPER_INGOT, 3) };

		// 플레이어 커스터마이징 데이터
		FCustomObjectData k_tCustomData;
	};
}
