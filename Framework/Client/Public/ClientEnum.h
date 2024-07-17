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
		BERRY_GOAT,		// ������Ʈ
		BOAR,			// ���� ����
		CARBUNCLO,		// ť������
		CHICKEN_PAL,	// ������
		DREAM_DEMON,	// ��������
		ELEC_PANDA,		// �Ϸ� �Ǵ�
		GRASS_MAMMOTH,	// �׸���
		HEDGE_HOG,		// ���ġ
		HEDGE_HOG_ICE,	// ��ġ��ġ
		KITSUNEBI,		// ����ȣ
		PENGUIN,		// ��Ű
		PINK_CAT,		// ��γ�
		SHEEP_BALL,		// ���η�
		WOOL_FOX,		// ����

		FENGYUNDEEPER,	// ������
		JETDRAGON,		// ��Ʈ����
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
		KINDLE,			// �� �ǿ��
		PLANT,			// ����
		// WATER,		// ����(������ �����մϴ�.)
		HANDIWORK,		// ���۾�
		LUMBER,			// ����
		TRANSPORT,		// ���
		ELECTRICITY,	// ����
		// GATHER,		// ä��(ä���� �����ϴ�.)
		MINE,			// ä��
		COOL,			// �ð�
		// FARM,		// ����
		ENUM_END
	};

    enum class EMonsterElementType
    {
        // �⺻ �Ӽ�
        GRASS,
        GROUND,
        ELECTRIC,
        WATER,
        FIRE,
        ICE,
        DRAGON,
        DARK,
        NEUTRAL,

		// Ư�� �Ӽ�
		WATER_ICE,		// ��Ű
		ICE_DRAGON,		// ����
		DRAGON_WATER,	// ��η�, ������
		FIRE_DARK,		// ����
		ENUM_END
	};

	enum class EPartnerSkillType
	{
		FLUFFY_SHIELD,			// ���η�	(��Ƽ�� ��ų)
		CAT_HELPER,				// ��γ�
		COLD_BOMB,				// ��ġ��ġ
		EGG_LAYER,				// ������
		LIFMUNK_RECOIL,			// ť�긮��	(��Ƽ�� ��ų)
		HUGGY_FIRE,				// ����ȣ	(��Ƽ�� ��ų)
		PENGULLET_CANNON,		// ��Ű		(��Ƽ�� ��ų)
		JOLT_BOMB,				// ���ġ	(��Ƽ�� ��ų)
		DREAM_CHASER,			// ��������	
		HARD_HEAD,				// ��������	(��Ƽ�� ��ų)
		BERRY_PICKER,			// ������Ʈ
		GAIA_CRUSHER,			// �׸��� (��Ƽ�� ��ų)
		FLUFFY_WOOL,			// ����

		WRIGGLING_WEASEL,		// ����
		HUNGRY_MISSILE,			// ��η�
		GUARDIAN_OF_THE_DESERT,	// �ƴ���
		MODIFIED_DNA,			// ����׸���
		AERIAL_MISSILE,			// ��Ʈ����
		STORMBRINGER_SEA_DRAGON,// ������
		YELLOW_TANK,			// �Ϸ��Ǵ�
		FLAME_ORGAN,			// ����
		LEATHER,				// ����

        // ��, ��, �� ���̵� ��ų�Դϴ�.
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
		DAY,	// ���༺
		NIGHT,	// ���༺
		ENUM_END
	};
	
	enum class EMonsterNoticeType
	{
		EXCLAMATION,// ����ǥ
		QUESTION,	// ����ǥ
		SURPRISED,	// ���
		LIGHT_BULB,	// ����
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
		BOOL_ATTACK,			// ���� �������� ����
		BOOL_ATTACKED_TARGET,	// ����� �����ߴ��� ����
		BOOL_CAPTURED,			// �� ���Ǿ �¾Ҵ��� ����
		BOOL_CAPTURE_RESULT,	// ��ȹ ���
		
		BOOL_DAMAGED,
		BOOL_DAY,
		BOOL_DEAD,
		BOOL_EMOTE,
		BOOL_ESCAPE,			// �� ���Ǿ� Ż�� ����
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
		PET,		// ���ٵ��
		SLAUGHTER,	// ����
		EAT,		// �����ֱ�
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
		CAMPFIRE,			// ��ں�
		BERRY_FARM,			// ���� ���

		LUMBERYARD,			// ������
		STONE_PIT,			// ä����
		CRUSHER,			// �ļ��

		ADVANCED_WORKBENCH,	// ��� �۾���
		BLAST_FURNACE,		// ȭ��
		SPHERE_TABLE,		// ���Ǿ� ���۴�

		WHEAT_FARM,			// �г��
		MILL,
		POT,
		WEAPON_WORKBENCH,	// ���� ���۴�

		// TODO; �̰��� �������� ������ �� �ִ� ���๰�� �ۼ��ؾ� �մϴ�.
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
		HAND_HELD_TORCH,	// ȶ��
		STONE_SPEAR,		// ��â
		MEAT_CLEAVER,		// Ǫ�ְ� Į
		OLD_BOW,			// Ȱ
		ROCKET_LAUNCHER,	// ���� ��ó
		STONE_AXE,			// ������
		STONE_PICKAXE,		// �����
		ASSAULT_RIFLE,		// ���� ����
		CONSTRUCT_HAMMER,	// ���ۿ� ��ġ
		ARROW,				// ȭ��
		BLUE_SPHERE,		// ��� ���Ǿ�
		GREEN_SPHERE,		// �׸� ���Ǿ�
		FRYING_PAN,			// ��������

		/* armor type */
		Outfit_OldCloth001,			// �⺻ ��
		Outfit_Cloth001,			// õ ��
		Outfit_Hunter001,			// ���׿�
		Outfit_Platinum001_v02,		// �ְ� ��-�ѷ�
		HeadEquip016,				// ��������
		HeadEquip018,				// ��ũ����
		HeadEquip019,				// �˷ϻ�����
		HeadEquip021,				// ��ŷ����
		HeadEquip029,				// ���ηո���
		HeadEquip033,				// ��γɸ���
		COMMON_SHIELD,				// �⺻ ����
		GIGA_SHIELD,				// �Ⱑ ����
		GLIDER,						// �۶��̴�


		/* mineral type */
		PALDIUM_FRAGMENT,	// �� ���� ����
		GOLD_COIN,			// ��ȭ
		STONE,				// ��
		WOOD,				// ����
		FIBER,				// ����
		LEATHER,			// ����
		COAL,				// ��ź
		WOOL,				// ����
		IRON,				// ö
		CLOTH,				// õ
		COPPER_INGOT,		// �ݼ� �ֱ�
		IRON_INGOT,			// ���� �ֱ�
		GUNPOWDER,			// ȭ��
		CARBON,				// ī������
		RIFLE_BULLET,		// ź��


		/* ingredient type*/
		EGG,				// ��
		SHEEPBALL_MEAT,		// ���η�
		BOAR_MEAT,			// ������
		RED_BERRY,			// ���� ����
		BAKED_BERRY,		// ���� ����
		FRIED_EGG,			// ����Ķ���
		SHEEPBALL_KEBAB,	// ���η� ��ġ
		GRASSMAMMOTH_MEAT,	// �׸��� ���
		SHEEPBALL_ROAST,	// ���η� ����
		CHICKEN_ROAST,		// ������ ����

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

		HAND_HELD_TORCH = 1, // ȶ��
		STONE_SPEAR,		 // ��â
		MEAT_CLEAVER,		 // Ǫ�ְ� Į
		OLD_BOW,			 // Ȱ
		ROCKET_LAUNCHER,	 // ���� ��ó
		STONE_AXE,			 // ������
		STONE_PICKAXE,		 // �����
		ASSAULT_RIFLE,		 // ���� ����
		CONSTRUCT_HAMMER,	 // ���ۿ� ��ġ
		ARROW,				 // ȭ��
		BLUE_SPHERE,		 // ��� ���Ǿ�
		GREEN_SPHERE,		 // �׸� ���Ǿ�
		FRYING_PAN,			 // ��������

		ENUM_END
	};

	enum class EWeaponActionType
	{
		// �ָ�
		NONE,
		// â
		SPEAR,
		// �׳� �ٰŸ�
		MELEE,
		// ������ Ȱ
		BOW,
		// ���� �߻��
		ROCKET_LAUNCHER,
		// ������ ����
		PROJECTILE,
		// ���ݼ���
		ASSAULT_RIFLE,

        ENUM_END
    };

	enum class EArmorType
	{
		NONE,

		Outfit_OldCloth001 = 14,	// �⺻ ��
		Outfit_Cloth001,			// õ ��
		Outfit_Hunter001,			// ���׿�
		Outfit_Platinum001_v02,		// �ְ� ��-�ѷ�
		HeadEquip016,				// ��������
		HeadEquip018,				// ��ũ����
		HeadEquip019,				// �˷ϻ�����
		HeadEquip021,				// ��ŷ����
		HeadEquip029,				// ���ηո���
		HeadEquip033,				// ��γɸ���
		COMMON_SHIELD,				// �⺻ ����
		GIGA_SHIELD,				// �Ⱑ ����
		GLIDER,						// �۶��̴�

		ENUM_END
	};

	enum class EMineralType
	{
		NONE,

		PALDIUM_FRAGMENT = 27,	// �� ���� ����
		GOLD_COIN,				// ��ȭ
		STONE,					// ��
		WOOD,					// ����
		FIBER,					// ����
		LEATHER,				// ����
		COAL,					// ��ź
		WOOL,					// ����
		IRON,					// ö
		CLOTH,					// õ
		COPPER_INGOT,			// �ݼ� �ֱ�
		IRON_INGOT,				// ���� �ֱ�
		GUNPOWDER,				// ȭ��
		CARBON,					// ī������
		RIFLE_BULLET,			// ź��

		ENUM_END
	};

	enum class EIngredientType
	{
		NONE,

		EGG = 42,			// ��
		SHEEPBALL_MEAT,		// ���η�
		BOAR_MEAT,			// ���� ���� ���
		RED_BERRY,			// ���� ����
		BAKED_BERRY,		// ���� ����
		FRIED_EGG,			// ����Ķ���
		SHEEPBALL_KEBAB,	// ���η� ��ġ
		CHICKEN_KEBAB,		// ������ ��ġ
		SHEEPBALL_ROAST,	// ���η� ����
		CHICKEN_ROAST,		// ������ ����

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
		// �۾���
		WORKBENCH,

		/* 2 */
		// �� ����, ��ں�, ���� ����, ������
		PAL_BOX, CAMPFIRE, WOODEN_CHEST, REPAIR_BENCH,
		// ���� ���, ���� ��, ���� ����, ���� ���
		// ���� ��, â�� ���� ���� ��, ���� �ﰢ ��, ���� ��� ����
		WOODEN_FOUNDATION, WOODEN_WALL, WOODEN_ROOF, WOODEN_STAIRS,
		WOODEN_DOOR, WOODEN_WINDOW_WALL, WOODEN_TRYANGLE_WALL, WOODEN_SLOPE_ROOF,

        /* 3 */
        // ħ��, �� ħ��, ���� ����
        HUMAN_BED, PAL_BED, FEEDING_BOX,

        /* 4 */
        // ���� ����, ���� ���(��)
        RANCH, BERRY_FARM,

		/* 5 */
		// ������, ä����, �ļ��
		LUMBERYARD, QUARRY, CRUSHER,

        /* 6 */
        // ��� �۾���, ȭ��, ���Ǿ� ���۴�
        ADVANCED_WORKBENCH, BLAST_FURNACE, SPHERE_TABLE,

        /* 7 */
        // �� ���, ���б�, �ݼ� ����, ���� �溮, ���� ����
        WHEAT_FARM, MILL, IRON_BOX, WOODEN_BARRIER, POT,

        /* 8 */
        // ���� ���۴�(�ѱ� ����)
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
		TREE,		// ����
		ROCK,		// ����
		BRANCH,		// ��������
		STONE,		// ������
		ENUM_END
	};

    enum class EBuildType
    {
        NORMAL,      // �Ϲ�
        FOUNDATION,   // ���
        WALL,      // ��
        STAIRS,      // ���
        ROOF,      // ����
        SLOPEDROOF,   // ����� ����
        DOOR,      // ��
        ENUM_END
    };

    enum class EBuildDir
    {
        /* ���� ���� */
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
