#pragma once

namespace Client
{
    /********************
       Client Enum
    ********************/
    enum class ECreatureType
    {
        PLAYER,
        PAL,
        ENUM_END
    };
	
	/********************
		Monster Enum
	********************/
	enum class EMonsterType
	{
		BERRY_GOAT,		// 베리고트
		BOAR,			// 돌진 돼지
		CARBUNCLO,		// 큐를리스
		CHICKEN_PAL,	// 꼬꼬닭
		DREAM_DEMON,	// 몽마둥이
		ELEC_PANDA,		// 일렉 판다
		GRASS_MAMMOTH,	// 그린모스
		HEDGE_HOG,		// 찌릿도치
		HEDGE_HOG_ICE,	// 코치도치
		KITSUNEBI,		// 파이호
		PENGUIN,		// 펭키
		PINK_CAT,		// 까부냥
		SHEEP_BALL,		// 도로롱
		WOOL_FOX,		// 밀피

		FENGYUNDEEPER,	// 윈드디어
		JETDRAGON,		// 제트래곤
		ENUM_END
	};

	enum class EMonsterSizeType
	{
		LARGE,
		MIDUM,
		SMALL,
		ENUM_END
	};

	enum class EMonsterTaskType
	{
		KINDLE,			// 불 피우기
		PLANT,			// 파종
		// WATER,		// 관개(파종과 같이합니다.)
		HANDIWORK,		// 수작업
		LUMBER,			// 벌목
		TRANSPORT,		// 운반
		ELECTRICITY,	// 발전
		// GATHER,		// 채집(채집은 없습니다.)
		MINE,			// 채굴
		COOL,			// 냉각
		// FARM,		// 목장
		ENUM_END
	};

    enum class EMonsterElementType
    {
        // 기본 속성
        GRASS,
        GROUND,
        ELECTRIC,
        WATER,
        FIRE,
        ICE,
        DRAGON,
        DARK,
        NEUTRAL,

		// 특수 속성
		WATER_ICE,		// 팽키
		ICE_DRAGON,		// 베비뇽
		DRAGON_WATER,	// 헤로롱, 레비드라
		FIRE_DARK,		// 사라블랙
		ENUM_END
	};

	enum class EPartnerSkillType
	{
		FLUFFY_SHIELD,			// 도로롱	(액티브 스킬)
		CAT_HELPER,				// 까부냥
		COLD_BOMB,				// 코치도치
		EGG_LAYER,				// 꼬꼬닭
		LIFMUNK_RECOIL,			// 큐룰리스	(액티브 스킬)
		HUGGY_FIRE,				// 파이호	(액티브 스킬)
		PENGULLET_CANNON,		// 팽키		(액티브 스킬)
		JOLT_BOMB,				// 찌릿도치	(액티브 스킬)
		DREAM_CHASER,			// 몽마둥이	
		HARD_HEAD,				// 돌진돼지	(액티브 스킬)
		BERRY_PICKER,			// 베리고트
		GAIA_CRUSHER,			// 그린모스 (액티브 스킬)
		FLUFFY_WOOL,			// 밀피

		WRIGGLING_WEASEL,		// 베비뇽
		HUNGRY_MISSILE,			// 헤로롱
		GUARDIAN_OF_THE_DESERT,	// 아누비스
		MODIFIED_DNA,			// 제노그리프
		AERIAL_MISSILE,			// 제트래곤
		STORMBRINGER_SEA_DRAGON,// 레비드라
		YELLOW_TANK,			// 일렉판다
		FLAME_ORGAN,			// 사라블랙
		LEATHER,				// 사라블랙

        // 육, 해, 공 라이딩 스킬입니다.
        RIDING_GROUND,
        RIDING_SWIMMING,
        RIDING_FLYING,
        ENUM_END
    };

	enum class EMonsterAIType
	{
		WILD_PAL,
		PARTNER_PAL,
		SLAVE_PAL,
		ENUM_END
	};

    enum class EMonsterPersonalityType
    {
        AGGRESSIVE,
        STUPID,
        PEACEFUL,
        ENUM_END
    };

	enum class EMonsterActionTimeType
	{
		DAY,	// 주행성
		NIGHT,	// 야행성
		ENUM_END
	};
	
	enum class EMonsterNoticeType
	{
		EXCLAMATION,// 느낌표
		QUESTION,	// 물음표
		SURPRISED,	// 놀람
		LIGHT_BULB,	// 전구
		ENUM_END
	};

	enum class ERidingObjectType
	{
		WIND_DEER,
		JET_DRAGON,
		ENUM_END
	};

	/********************
		Blackboard enum
	********************/
	enum class EBlackboardBoolValue
	{
		// bool
		BOOL_AGGRO,
		BOOL_ATTACK,			// 공격 상태인지 여부
		BOOL_ATTACKED_TARGET,	// 대상을 공격했는지 여부
		BOOL_CAPTURED,			// 팰 스피어에 맞았는지 여부
		BOOL_CAPTURE_RESULT,	// 포획 결과
		
		BOOL_DAMAGED,
		BOOL_DAY,
		BOOL_DEAD,
		BOOL_EMOTE,
		BOOL_ESCAPE,			// 팰 스피어 탈출 여부
		BOOL_FEEDED,
		BOOL_HOLD_POSITION,
		BOOL_HUNGER,
		BOOL_INTERACTED,
		BOOL_JUMP,
		BOOL_PARTNER_SKILL,
		BOOL_REACT,
		BOOL_RUNAWAY,
		BOOL_SLEEP,
		BOOL_TODO_START,
		BOOL_TODO_PAUSE,
		BOOL_TODO_FINISH,

		ENUM_END
	};

	enum class EBlackboardIntValue
	{
		// int
		INT_INTERACT_TYPE,
		INT_SKILL_TYPE,
		INT_NUM_SHAKES,
		ENUM_END
	};

	enum class EBlackboardFloatValue
	{
		// float
		FLOAT_DELTA_SECONDS,
		FLOAT_ACCEPTABLE_RADIUS,
		ENUM_END
	};

	enum class EBlackboardVector3Value
	{
		// vector3
		VECTOR3_RANDOM_PLACE,
		// VECTOR3_TODO_PLACE,
		// VECTOR3_PLAYER_PLACE,
		ENUM_END
	};

	enum class EBlackboardGameObjectValue
	{
		// gameobject
		TODO_TARGET,
		ATTACK_TARGET,
		PLAYER_TARGET,
		EFFECT_GROUP_00,
		EFFECT_GROUP_01,
		EFFECT_GROUP_02,
		EFFECT_GROUP_03,
		ENUM_END
	};

	enum class EMonsterInteractType
	{
		PET,		// 쓰다듬기
		SLAUGHTER,	// 도축
		EAT,		// 먹이주기
		ENUM_END,
	};

	enum class EPalEyeType
	{
		DEFAULT,	// 0, 0
		TIRED,		// 0, 1
		CLOSED,		// 0, 2
		DAMAGED,	// 0, 3

		SMIZED,		// 1, 0
		ANGRY,		// 1, 2
		CRY,		// 1, 3
		FAINT,		// 1, 4
	};

	enum class EPalMouthType
	{
		DEFAULT,	// 0, 0
		DAMAGED,	// 0, 1
		SMILE,		// 1, 0
		ANGRY		// 1, 1
	};

	/********************
		Factory Enum
	********************/
	enum class EFactoryType
	{
		WORK_BENCH,
		REPAIR_BENCH,
		CAMPFIRE,			// 모닥불
		BERRY_FARM,			// 열매 농원

		LUMBERYARD,			// 벌목장
		STONE_PIT,			// 채석장
		CRUSHER,			// 파쇄기

		ADVANCED_WORKBENCH,	// 상급 작업대
		BLAST_FURNACE,		// 화로
		SPHERE_TABLE,		// 스피어 제작대

		WHEAT_FARM,			// 밀농원
		MILL,
		POT,
		WEAPON_WORKBENCH,	// 무기 제작대

		// TODO; 이곳에 아이템을 제작할 수 있는 건축물을 작성해야 합니다.
		ENUM_END
	};

	enum class EToDoType
	{
		BUILD_ARCHITECTURE,
		PRODUCE_ITEM,
		CUT_DOWN_RAW_TREE,
		CUT_DOWN_LUMBERYARD,
		MINE_RAW_ROCK,
		MINE_STONE_PIT,
		TRANSPORT_MINERAL,
		TRANSPORT_INGREDIENT,
		
		FARM_BERRY,
		COOK_FOOD,
		ENUM_END
	};

	enum class EMonsterToDoState
	{
		NO_WORK,
		REST,
		// SLEEP,
		WORK,
		ENUM_END
	};

	enum class EToDoCommandState
	{
		NOT_ASSIGNED,
		STARTED,
		PAUSED,
		FINISHED,
		ENUM_END
	};

	enum class EProduceItemState
	{
		NOT_COMPLETED,
		PARTIALY_COMPLETED,
		COMPLETED,
		ENUM_END
	};

	/********************
		Item Enum
	********************/
	enum class EItemType
	{
		NONE,

		/* weapon type */
		HAND_HELD_TORCH,	// 횃불
		STONE_SPEAR,		// 돌창
		MEAT_CLEAVER,		// 푸주간 칼
		OLD_BOW,			// 활
		ROCKET_LAUNCHER,	// 로켓 런처
		STONE_AXE,			// 돌도끼
		STONE_PICKAXE,		// 돌곡괭이
		ASSAULT_RIFLE,		// 돌격 소총
		CONSTRUCT_HAMMER,	// 제작용 망치
		ARROW,				// 화살
		BLUE_SPHERE,		// 블루 스피어
		GREEN_SPHERE,		// 그린 스피어
		FRYING_PAN,			// 프라이팬

		/* armor type */
		Outfit_OldCloth001,			// 기본 옷
		Outfit_Cloth001,			// 천 옷
		Outfit_Hunter001,			// 가죽옷
		Outfit_Platinum001_v02,		// 최고 방어구-한랭
		HeadEquip016,				// 브론즈모자
		HeadEquip018,				// 테크모자
		HeadEquip019,				// 알록새모자
		HeadEquip021,				// 펭킹모자
		HeadEquip029,				// 도로롱모자
		HeadEquip033,				// 까부냥모자
		COMMON_SHIELD,				// 기본 쉴드
		GIGA_SHIELD,				// 기가 쉴드
		GLIDER,						// 글라이더


		/* mineral type */
		PALDIUM_FRAGMENT,	// 팰 지움 파편
		GOLD_COIN,			// 금화
		STONE,				// 돌
		WOOD,				// 나무
		FIBER,				// 섬유
		LEATHER,			// 가죽
		COAL,				// 석탄
		WOOL,				// 양털
		IRON,				// 철
		CLOTH,				// 천
		COPPER_INGOT,		// 금속 주괴
		IRON_INGOT,			// 제련 주괴
		GUNPOWDER,			// 화약
		CARBON,				// 카본섬유
		RIFLE_BULLET,		// 탄약


		/* ingredient type*/
		EGG,				// 알
		SHEEPBALL_MEAT,		// 도로롱
		BOAR_MEAT,			// 꼬꼬닭
		RED_BERRY,			// 빨간 열매
		BAKED_BERRY,		// 구운 열매
		FRIED_EGG,			// 계란후라이
		SHEEPBALL_KEBAB,	// 도로롱 꼬치
		GRASSMAMMOTH_MEAT,	// 그린모스 고기
		SHEEPBALL_ROAST,	// 도로롱 구이
		CHICKEN_ROAST,		// 꼬꼬닭 구이

		//BROAD_SWORD,
		ENUM_END

	};

	enum class EItemKindType
	{
		WEAPON,
		ARMOR,
		MINERAL,
		INGREDIENT,
		ENUM_END
	};

	enum class EWeaponType
	{
		NONE,

		HAND_HELD_TORCH = 1, // 횃불
		STONE_SPEAR,		 // 돌창
		MEAT_CLEAVER,		 // 푸주간 칼
		OLD_BOW,			 // 활
		ROCKET_LAUNCHER,	 // 로켓 런처
		STONE_AXE,			 // 돌도끼
		STONE_PICKAXE,		 // 돌곡괭이
		ASSAULT_RIFLE,		 // 돌격 소총
		CONSTRUCT_HAMMER,	 // 제작용 망치
		ARROW,				 // 화살
		BLUE_SPHERE,		 // 블루 스피어
		GREEN_SPHERE,		 // 그린 스피어
		FRYING_PAN,			 // 프라이팬

		ENUM_END
	};

	enum class EWeaponActionType
	{
		// 주먹
		NONE,
		// 창
		SPEAR,
		// 그냥 근거리
		MELEE,
		// 오래된 활
		BOW,
		// 로켓 발사기
		ROCKET_LAUNCHER,
		// 던지는 무기
		PROJECTILE,
		// 돌격소총
		ASSAULT_RIFLE,

        ENUM_END
    };

	enum class EArmorType
	{
		NONE,

		Outfit_OldCloth001 = 14,	// 기본 옷
		Outfit_Cloth001,			// 천 옷
		Outfit_Hunter001,			// 가죽옷
		Outfit_Platinum001_v02,		// 최고 방어구-한랭
		HeadEquip016,				// 브론즈모자
		HeadEquip018,				// 테크모자
		HeadEquip019,				// 알록새모자
		HeadEquip021,				// 펭킹모자
		HeadEquip029,				// 도로롱모자
		HeadEquip033,				// 까부냥모자
		COMMON_SHIELD,				// 기본 쉴드
		GIGA_SHIELD,				// 기가 쉴드
		GLIDER,						// 글라이더

		ENUM_END
	};

	enum class EMineralType
	{
		NONE,

		PALDIUM_FRAGMENT = 27,	// 팰 지움 파편
		GOLD_COIN,				// 금화
		STONE,					// 돌
		WOOD,					// 나무
		FIBER,					// 섬유
		LEATHER,				// 가죽
		COAL,					// 석탄
		WOOL,					// 양털
		IRON,					// 철
		CLOTH,					// 천
		COPPER_INGOT,			// 금속 주괴
		IRON_INGOT,				// 제련 주괴
		GUNPOWDER,				// 화약
		CARBON,					// 카본섬유
		RIFLE_BULLET,			// 탄약

		ENUM_END
	};

	enum class EIngredientType
	{
		NONE,

		EGG = 42,			// 알
		SHEEPBALL_MEAT,		// 도로롱
		BOAR_MEAT,			// 돌진 돼지 고기
		RED_BERRY,			// 빨간 열매
		BAKED_BERRY,		// 구운 열매
		FRIED_EGG,			// 계란후라이
		SHEEPBALL_KEBAB,	// 도로롱 꼬치
		CHICKEN_KEBAB,		// 꼬꼬닭 꼬치
		SHEEPBALL_ROAST,	// 도로롱 구이
		CHICKEN_ROAST,		// 꼬꼬닭 구이

		ENUM_END
	};

    enum class EPalsphereType
    {
        BLUE,
        YELLOW,
        GREEN,
        RED,
        PINK,
        PURPLE,
        ENUM_END
    };

    enum class EPlayerControllerMode
    {
        DEFAULT_MODE,
        STATICMOVE_MODE,
        NONEMOVE_MODE,
        CLIMB_MODE
    };

	/********************
	   Habitat Enum
	********************/
	enum class EArchitectureType
	{
		/* 1 */
		// 작업대
		WORKBENCH,

		/* 2 */
		// 팰 상자, 모닥불, 나무 상자, 수리대
		PAL_BOX, CAMPFIRE, WOODEN_CHEST, REPAIR_BENCH,
		// 나무 토대, 나무 벽, 나무 지붕, 나무 계단
		// 나무 문, 창문 딸린 나무 벽, 나무 삼각 벽, 나무 경사 지붕
		WOODEN_FOUNDATION, WOODEN_WALL, WOODEN_ROOF, WOODEN_STAIRS,
		WOODEN_DOOR, WOODEN_WINDOW_WALL, WOODEN_TRYANGLE_WALL, WOODEN_SLOPE_ROOF,

        /* 3 */
        // 침대, 팰 침대, 먹이 상자
        HUMAN_BED, PAL_BED, FEEDING_BOX,

        /* 4 */
        // 가축 목장, 열매 농원(밭)
        RANCH, BERRY_FARM,

		/* 5 */
		// 벌목장, 채석장, 파쇄기
		LUMBERYARD, QUARRY, CRUSHER,

        /* 6 */
        // 상급 작업대, 화로, 스피어 제작대
        ADVANCED_WORKBENCH, BLAST_FURNACE, SPHERE_TABLE,

        /* 7 */
        // 밀 농원, 제분기, 금속 상자, 나무 방벽, 조리 냄비
        WHEAT_FARM, MILL, IRON_BOX, WOODEN_BARRIER, POT,

        /* 8 */
        // 무기 제작대(총기 제작)
        WEAPON_WORKBENCH,

		ENUM_END
	};

	enum class EHabitatType
	{
		NATURE,
		PAL,
		ARCHITECTURE,
		SPOTLIGHT,
		POINTLIGHT,
		DIRECTIONALLIGHT,
		CAMERA,
		ENUM_END
	};

	enum class ENatureType
	{
		TREE,		// 나무
		ROCK,		// 바위
		BRANCH,		// 나뭇가지
		STONE,		// 돌멩이
		ENUM_END
	};

    enum class EBuildType
    {
        NORMAL,      // 일반
        FOUNDATION,   // 토대
        WALL,      // 벽
        STAIRS,      // 계단
        ROOF,      // 지붕
        SLOPEDROOF,   // 경사진 지붕
        DOOR,      // 문
        ENUM_END
    };

    enum class EBuildDir
    {
        /* 건축 방향 */
        UP,
        DOWN,
        LOOK,      // Z+
        RIGHT,      // X+
        LEFT,      // X-
        BACK,      // Z-
        LOOK_UP,
        LOOK_DOWN,
        RIGHT_UP,
        RIGHT_DOWN,
        BACK_UP,
        BACK_DOWN,
        LEFT_UP,
        LEFT_DOWN,
        ENUM_END
    };

	/********************
		Event Enum
	********************/
	enum class ELevelType
	{
		PRE_LOAD	= 0,
		LOAD		= 1,
		LOGO		= 2,
		PERSISTENT	= 3,
		SAMPLE		= 4,
		ISLAND		= 5,
		TOWER		= 6,
		CUSTOMIZE	= 7,
		ENUM_END
	};

	/********************
		Sorting Enum
	********************/
	enum class ESortingType
	{
		RANDOM,
		ITEM_TYPE,
		ITEM_COUNT,
		MONSTER_TYPE,
		ENUM_END
	};

	/********************
		ArchitectureSystem Enum
	********************/
	enum class EArchitectureSystemType
	{
		ARCHITECTURE,
		GROCERIES,
		INFRA,
		PRODUCTION,
		ENUM_END
	};

	/********************
		ProductionSystem Enum
	********************/
	enum class EProductionSystemType
	{
		WORKBENCH,
		STOVE,
		FOOD,
		EQUIPMENT,
		ENUM_END
	};


	/************************
		Equipped Item Enum
	************************/
	enum class EEquippedItemType
	{
		WEAPON,
		ACCESSORY,
		FOOD,
		HEAD,
		BODY,
		SHIELD,
		GLIDER,
		ENUM_END
	};

	/************************
		Slot Enum
	************************/
	enum class ESlotType
	{
		FIRST,
		SECOND,
		THIRD,
		ENUM_END
	};

	/************************
		Player Emote Enum
	************************/
	enum class EEmoteType
	{
		WAVE, 
		DANCE, 
		CHEER,
		ENUM_END 
	};

	enum class ENpcModelType
	{
		NPC_Female_People02,
		NPC_Female_People03,
		NPC_Female_Soldier04,
		NPC_Male_Believer01,
		NPC_Male_DesertPeople01,
		NPC_Male_People02,
		NPC_Male_Police00,
		NPC_Male_Trader03,
		NPC_Outfit_GrassBoss001_ALL,
		ENUM_END
	};
}
