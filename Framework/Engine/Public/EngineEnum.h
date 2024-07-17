#pragma once

namespace Engine
{
	/********************
		Engine Enum
	********************/
	enum class ELevelState
	{
		INITIALIZING,
		INITIALIZED,
		BEGIN_PLAYING,
		BEGIN_PLAYED,
		PRE_TICKING,
		PRE_TICKED,
		TICKING,
		TICKED,
		POST_TICKING,
		POST_TICKED,
		CLEARING,
		CLEARED,
		RELEASING,
		RELEASED,
		ENUM_END
	};

	enum class ETransformType
	{
		RIGHT,
		UP,
		FORWARD,
		POSITION,
		ENUM_END,
	};

	enum class EDateType
	{
		DATE_60,
		DATE_120,
		DATE_180,
		DATE_300,	// 5분
		DATE_600,	// 10분
		DATE_1200,	// 20분
		ENUM_END
	};

	enum class ERenderType
	{
		NONE,
		RENDER_PRE,
		RENDER_NBLEND,
		RENDER_LBLEND,
		RENDER_PBR,
		RENDER_BLEND,
		RENDER_UI,
		RENDER_POST,
		RENDER_DEBUG,
		RENDER_GLOW,
		RENDER_DISTORTION,

		RENDER_LBLEND_REVERSE,
		RENDER_LBLEND_ADD,
		RENDER_BLEND_REVERSE,
		RENDER_BLEND_ADD,

		RENDER_NBLEND_NONCULL,
		RENDER_LBLEND_NONCULL,
		RENDER_LBLEND_REVERSE_NONCULL,
		RENDER_LBLEND_ADD_NONCULL,
		RENDER_NLIGHT,
		RENDER_NLIGHT_NONCULL,
		RENDER_BLEND_NONCULL,
		RENDER_BLEND_REVERSE_NONCULL,
		RENDER_BLEND_ADD_NONCULL,

		RENDER_POST_UI,
		RENDER_MOSAIC,
		ENUM_END
	};

	enum class EShaderPath
	{
		DEFAULT,
		NONCULL,
		ALPHABLEND,
		NONCULL_ALPHABLEND,
		REVERSE_ALHPABLEND,
		NONCULL_REVERSE_ALHPABLEND,
		BLEND,
		NONCULL_BLEND,
		ENUM_END
	};

	enum class EShaderType
	{
		DEFAULT_ANIM,			// 유니크 애님 모델
		DEFAULT_NONANIM,		// 유니크 논애님 모델
		DEFAULT_INSTANCE,		// 인스턴스 논애님 모델
		PLAYEREYE_ANIM,			// 유니크 애님 모델 -> 플레이어 눈
		MONSTERFACE_ANIM,		// 유니크 애님 모델 -> 몬스터 표정
		MONSTER_KITSUNEBI_FIRE,
		PALLIT_TOP_NONANIM,		// 유니크 논애님 상부 혼합 모델
		PALLIT_TOP_INSTANCE,	// 인스턴스 논애님 상부 혼합 모델
		EXTERN_NONANIM,
		EXTERN_INSTANCE,		// 인스턴스 논애님 외부 에셋 모델
		LANDSCAPE_NONANIM,		// 지형
		SKY_NONANIM,			// 스카이
		ARCHITECTURE,
		NONE,					// 해당 없음
		ENUM_END				// 기본 값
	};

	enum class EViewType
	{
		BACKBUFFER,
		BACKBUFFER_COPY,
		DIFFUSE,
		NORMAL,
		MRSO,
		METALIC,	// 디버그용
		ROUGHNESS,	// 디버그용
		SPECULAR,	// 디버그용
		OCCLUSION,	// 디버그용
		EMISSION,
		DEPTH,
		WORLDPOSITION,
		OBJECTID,
		GLOW,
		BLOOM,
		BLUR_X,
		BLUR_Y,
		GLOW_RESULT,
		DISTORTION,
		DISTORTION_RESULT,
		MOSAIC,
		MOSAIC_RESULT
	};

	enum class EPipelineViewType
	{
		DNMEDW,
		BACKBUFFER,
		GLOW,
		BLOOM,
		BLUR_X,
		BLUR_Y,
		GLOW_RESULT,
		DISTORTION,
		DISTORTION_RESULT,
		MOSAIC,
		MOSAIC_RESULT
	};

	enum class ECbufferType
	{
		FRAME = 0,
		VIEW = 1,
		SHADERINFO = 2,
		SHADERINFO_CHILD = 3,
		RW = 4,
	};

	enum class EModelType
	{
		ANIM,					// 애니메이션 모델
		NONANIM,				// 일반
		MAP_LANDSCAPE,			// 지형
		EXTERN,					// 외부 에셋 모델
		MAP_OBJECT,				// 환경
		MAP_OBJECT_COLLISION,	// 환경 충돌체
		MAP_DEBUG,				// 디버그용 맵
		ENUM_END
	};

	enum class EMeshManageType
	{
		UNIQUE,							// 고유 모델, Mesh Handler에 등록되지 않으며, 소유한 Component에서 직접 모든 바인드를 해 주어야 한다.
		INSTANCE,						// 인스턴스 모델. Mesh Handler에 등록되어 인스턴싱되어 그려진다.
		ENUM_END
	};

	enum class EArchitextureState
	{
		BLUEPRINT,	// 설치 준비
		BUILDING,	// 건설중
		COMPLETE,	// 완성
		ENUM_END
	};

	enum class EMaterialTextureType
	{

		ENUM_END
	};

	enum class EAutoBindType
	{
		AUTOBIND_MODEL_ANIM,
		AUTOBIND_MODEL_NONANIM,
		AUTOBIND_MODEL_LANDSCAPE,
		AUTOBIND_VFX_DISTORTION,
		AUTOBIND_VFX_DISSOLVE,
		AUTOBIND_VFX_BLOOM,
		AUTOBIND_VFX_GLOW,
		AUTOBIND_UI_DEFAULT,
		AUTOBIND_UI_BLUR,
		AUTOBIND_UI_BLOOM,
		AUTOBIND_UI_BLUR_BLOOM,

		AUTOBIND_DEBUG_DIFFUSE,

		ENUM_END
	};

	enum class EComponentType
	{
		TRANSFORM,
		CAMERA,
		MESH,
		SPHERE,
		BOX,
		CAPSULE,
		DIRECTIONAL_LIGHT,
		POINT_LIGHT,
		SPOT_LIGHT,
		RECT,
		PARTICLE,
		EFFECTMESH,
		HEIGHT_FIELD,
		BEHAVIOR_TREE,
		FSM,
		TRIANGLE_MESH,
		CONTROLLER,
		ENUM_END
	};

	enum class EProjectionType
	{
		ORTHOGONAL,
		PERSPECTIVE,
		ENUM_END
	};

	enum class ELightType
	{
		DIRECTIONAL,
		POINT,
		ENUM_END
	};

	enum class ECameraType
	{
		DEFAULT,	// 기본 카메라
		SPHERICAL,	// 구 카메라
		CINEMATIC,	// 연출 카메라
		ENUM_END
	};

	enum class EFrameLoopType
	{
		INNER,
		OUTER,
		ENUM_END
	};

	enum class EShakeType
	{
		SHAKE_ROTX,
		SHAKE_ROTY,
		SHAKE_ROTZ,
		SHAKE_RIGHT,
		SHAKE_UP,
		SHAKE_LOOK,
		SHAKE_FOV,
		ENUM_END
	};

	/********************
		UI Enum
	********************/
	enum class EUIType
	{
		STATIC_IMAGE,
		DYNAMIC_IMAGE,
		SLIDER,
		BUTTON,
		TEXT,
		LINEAR_HORIZONTAL_PROGRESSBAR,
		LINEAR_VERTICAL_PROGRESSBAR,
		CIRCULAR_PROGRESSBAR,
		LINEAR_INTERPOLATION_PROGRESSBAR,
		FRAME_IMAGE,
		CIRCLE_INTERPOLATION_PROGRESSBAR,
		ENUM_END
	};

	enum class EImageType
	{
		DEFAULT,
		INVENTORY,
		ITEM,
		ENUM_END
	};

	enum class EButtonState
	{
		HOVER,
		UP,
		ENUM_END
	};

	enum class ETextType
	{
		LEFT,
		CENTER,
		RIGHT,
		ENUM_END
	};

	/********************
		Effect Enum
	********************/
	enum class EEffectMode
	{
		LERP,
		KEYFRAME,
		ENUM_END
	};

	enum class EEffectType
	{
		PARTICLE,
		EFFECTMESH,
		InstanceEffectMesh,
		ENUM_END
	};

	enum class EParticleType
	{
		DROP,
		SPREAD,
		ERUPTION,
		CHARGE,
		ORTHOGONAL,
		REVOLUTION, // 공전
		FOLLOW,		// 부모를 따라다님
		TONADO,
		GROUNDED,
		ALPHADECREASE,
		SPRAY,
		ENUM_END
	};

	enum class ETextureType
	{
		UI,
		DIFFUSE,
		NOISE,
		MASK,
		DISTORTION,
		DISSOLVE,
		MODEL,
		ENUM_END
	};

	enum class EEasingType
	{
		EASE_NONE,
		EASE_LINEAR,
		EASE_IN_SINE,
		EASE_OUT_SINE,
		EASE_INOUT_SINE,
		EASE_IN_SQUARE,
		EASE_OUT_SQUARE,
		EASE_INOUT_SQUARE,
		EASE_IN_CIRC,
		EASE_OUT_CIRC,
		EASE_INOUT_CIRC,
		EASE_IN_BACK,
		EASE_OUT_BACK,
		EASE_INOUT_BACK,
		EASE_IN_ELASTIC,
		EASE_OUT_ELASTIC,
		EASE_INOUT_ELASTIC,
		EASE_IN_BOUNCE,
		EASE_OUT_BOUNCE,
		EASE_INOUT_BOUNCE,
		ENUM_END
	};

	/********************
		Sound Enum
	********************/
	enum class ESoundAttenuationType
	{
		LINEAR,
		ENUM_END
	};

	enum class EChannelGroupType
	{
		MASTER,
		BACKGROUND,
		EFFECT,
		INTERACTION,
		USER_INTERFACE,
		VOICE,
		ENVIRONMENT,
		ENUM_END
	};

	enum class EChannelType
	{
		// 배경 음악 2채널
		BACKGROUND_00 = 0,
		BACKGROUND_01,

		// 자동 바인드 26채널
		AUTO_00 = 2,
		AUTO_01,
		AUTO_02,
		AUTO_03,
		AUTO_04,
		AUTO_05,
		AUTO_06,
		AUTO_07,
		AUTO_08,
		AUTO_09,
		AUTO_10,
		AUTO_11,
		AUTO_12,
		AUTO_13,
		AUTO_14,
		AUTO_15,
		AUTO_16,
		AUTO_17,
		AUTO_18,
		AUTO_19,
		AUTO_20,
		AUTO_21,
		AUTO_22,
		AUTO_23,
		AUTO_24,
		AUTO_25,

		// 수동 바인드 4채널
		MANUAL_26,
		MANUAL_27,
		MANUAL_28,
		MANUAL_29,

		ENUM_END
	};

	/********************
		Event Enum
	********************/
	enum class EEventType
	{
		OPEN_LEVEL,
		ENUM_END
	};

	enum class EEventInvokeType
	{
		FORWARD,
		DEFERRED,
		ENUM_END
	};

	enum class ENotifyType
	{
		COLLISION,
		SOUND,
		EFFECT,
		FUNC,
		ENUM_END
	};

	/********************
		Controller Enum
	********************/
	enum class EControllerShapeType
	{
		CAPSULE,
		BOX,
		ENUM_END
	};

	enum class EVerticalMovementType
	{
		UP,
		DOWN,
		ON_GROUND,
		ENUM_END
	};

	enum class EHorizontalMovementType
	{
		FORWARD,
		BACKWARD,
		LEFT,
		RIGHT,
		ENUM_END
	};

	enum class EMobilityType
	{
		STATIC,
		DYNAMIC,
		ENUM_END
	};

	/********************
		HFSM Enum
	********************/
	enum class EHFSMReserveCommandType
	{
		FRONT,
		DEFERRED,
		BACK,
		ENUM_END
	};

	/********************
		BehaviorTree Enum
	********************/
	enum class EBlackboardValueType
	{
		BOOL,
		INT,
		FLOAT,
		VECTOR3,
		GAME_OBJECT,
		ENUM_END
	};

	enum class EBTNodeType
	{
		COMPOSITE,
		TASK,
		DECORATOR,
		ENUM_END
	};

	enum class EBTNodeResult
	{
		FAILED,
		SUCCEEDED,
		IN_PROGRESS,
		ABORT,

		DECO_FAILED,
		DECO_SUCCEEDED,
		ENUM_END
	};

	enum class ECompositeType
	{
		SELECTOR,
		SEQUENCE,
		PARALLEL,
		ENUM_END
	};

	/********************
		Shader Enum
	********************/
	/*
	enum class EShaderType
	{
		COMPUTE,
		DOMAIN_,// 이미 DOMAIN 매크로가 있습니다.
		PIXEL,
		GEOMETRY,
		HULL,
		VERTEX,
		ENUM_END
	};
	*/

	enum class EBufferType
	{
		VERTEX,
		PIXEL,
		CONSTANT,
		ENUM_END
	};

	enum class EConstantBufferType
	{
		ONE_INT,
		TWO_INT,
		THREE_INT,
		FOUR_INT,

		ONE_FLOAT,
		TWO_FLOAT,
		THREE_FLOAT,
		FOUR_FLOAT,

		ONE_MATRIX,
		TWO_MATRIX,
		THREE_MATRIX,
		FOUR_MATRIX,

		ONE_VECTOR3,
		TWO_VECTOR3,
		THREE_VECTOR3,
		FOUR_VECTOR3,

		ONE_VECTOR4,
		TWO_VECTOR4,
		THREE_VECTOR4,
		FOUR_VECTOR4,

		ENUM_END
	};

	/********************
		PhysX Enum
	********************/
	enum class EAxisLockType
	{
		LOCK_LINEAR_X = (1 << 0),
		LOCK_LINEAR_Y = (1 << 1),
		LOCK_LINEAR_Z = (1 << 2),
		LOCK_ANGULAR_X = (1 << 3),
		LOCK_ANGULAR_Y = (1 << 4),
		LOCK_ANGULAR_Z = (1 << 5),
		ENUM_END
	};

	enum class EColliderType
	{
		SPHERE,
		BOX,
		CAPSULE,
		HEIGHT_FIELD,
		TRIANGLE_MESH,
		ENUM_END
	};

	enum class ECollisionLayer
	{
		PLAYER = 1 << 1,
		BOSS = 1 << 2,
		ALLY = 1 << 3,
		MONSTER = 1 << 4,
		PLAYER_WEAPON = 1 << 5,
		ALLY_WEAPON = 1 << 6,
		MONSTER_WEAPON = 1 << 7,
		PLAYER_PROJECTILE = 1 << 8,
		ALLY_PROJECTILE = 1 << 9,
		MONSTER_PROJECTILE = 1 << 10,
		ITEM = 1 << 11,
		ENVIRONMENT = 1 << 12,
		BUILDING = 1 << 13,
		NATURE = 1 << 14,
		INTERACTION = 1 << 15,
		BASE_CAMP = 1 << 16,
		NPC = 1 << 17,
		RIDING = 1 << 18,
		BOWL = 1 << 19,
		BOWLINGPIN = 1 << 20,
		BOWLINGTRIGGER = 1 << 21,
		MINIGAME_GACHA = 1 << 22,
		BOWLING_FLOOR = 1 << 23,
		ENVIRONMENT2 = 1 << 24,
		ENUM_END
	};

	enum class ECollisionFlag
	{
		PLAYER
		= (int)ECollisionLayer::BOSS
		| (int)ECollisionLayer::MONSTER
		| (int)ECollisionLayer::MONSTER_WEAPON
		| (int)ECollisionLayer::MONSTER_PROJECTILE
		| (int)ECollisionLayer::ENVIRONMENT
		| (int)ECollisionLayer::ENVIRONMENT2
		| (int)ECollisionLayer::BUILDING
		| (int)ECollisionLayer::NPC
		| (int)ECollisionLayer::NATURE
		| (int)ECollisionLayer::MINIGAME_GACHA
		| (int)ECollisionLayer::BASE_CAMP,

		BOSS
		= (int)ECollisionLayer::PLAYER
		| (int)ECollisionLayer::ALLY
		| (int)ECollisionLayer::MONSTER
		| (int)ECollisionLayer::ALLY_WEAPON
		| (int)ECollisionLayer::PLAYER_WEAPON
		| (int)ECollisionLayer::ALLY_PROJECTILE
		| (int)ECollisionLayer::PLAYER_PROJECTILE
		| (int)ECollisionLayer::ENVIRONMENT
		| (int)ECollisionLayer::ENVIRONMENT2
		| (int)ECollisionLayer::BUILDING
		| (int)ECollisionLayer::NATURE,

		ALLY
		= (int)ECollisionLayer::BOSS
		| (int)ECollisionLayer::MONSTER
		| (int)ECollisionLayer::MONSTER_WEAPON
		| (int)ECollisionLayer::MONSTER_PROJECTILE
		| (int)ECollisionLayer::ENVIRONMENT
		| (int)ECollisionLayer::ENVIRONMENT2
		// | (int)ECollisionLayer::BUILDING
		| (int)ECollisionLayer::INTERACTION
		| (int)ECollisionLayer::NATURE,

		MONSTER
		= (int)ECollisionLayer::PLAYER
		| (int)ECollisionLayer::BOSS
		| (int)ECollisionLayer::ALLY
		| (int)ECollisionLayer::ALLY_WEAPON
		| (int)ECollisionLayer::PLAYER_WEAPON
		| (int)ECollisionLayer::ALLY_PROJECTILE
		| (int)ECollisionLayer::PLAYER_PROJECTILE
		| (int)ECollisionLayer::ENVIRONMENT
		| (int)ECollisionLayer::ENVIRONMENT2
		| (int)ECollisionLayer::BUILDING
		| (int)ECollisionLayer::NATURE,

		PLAYER_WEAPON
		= (int)ECollisionLayer::BOSS
		| (int)ECollisionLayer::MONSTER
		| (int)ECollisionLayer::ENVIRONMENT
		| (int)ECollisionLayer::ENVIRONMENT2
		| (int)ECollisionLayer::NATURE
		| (int)ECollisionLayer::BUILDING,

		ALLY_WEAPON
		= (int)ECollisionLayer::BOSS
		| (int)ECollisionLayer::MONSTER
		| (int)ECollisionLayer::ENVIRONMENT
		| (int)ECollisionLayer::ENVIRONMENT2
		| (int)ECollisionLayer::NATURE
		| (int)ECollisionLayer::BUILDING,

		MONSTER_WEAPON
		= (int)ECollisionLayer::PLAYER
		| (int)ECollisionLayer::ALLY
		| (int)ECollisionLayer::ENVIRONMENT
		| (int)ECollisionLayer::ENVIRONMENT2
		| (int)ECollisionLayer::NATURE
		| (int)ECollisionLayer::BUILDING,

		PLAYER_PROJECTILE
		= (int)ECollisionLayer::BOSS
		| (int)ECollisionLayer::MONSTER
		| (int)ECollisionLayer::ENVIRONMENT
		| (int)ECollisionLayer::ENVIRONMENT2
		| (int)ECollisionLayer::NATURE
		| (int)ECollisionLayer::BUILDING,

		ALLY_PROJECTILE
		= (int)ECollisionLayer::BOSS
		| (int)ECollisionLayer::MONSTER
		| (int)ECollisionLayer::ENVIRONMENT
		| (int)ECollisionLayer::ENVIRONMENT2
		| (int)ECollisionLayer::NATURE
		| (int)ECollisionLayer::BUILDING,

		MONSTER_PROJECTILE
		= (int)ECollisionLayer::PLAYER
		| (int)ECollisionLayer::ALLY
		| (int)ECollisionLayer::ENVIRONMENT
		| (int)ECollisionLayer::ENVIRONMENT2
		| (int)ECollisionLayer::NATURE
		| (int)ECollisionLayer::BUILDING,

		ITEM
		= (int)ECollisionLayer::INTERACTION
		| (int)ECollisionLayer::BASE_CAMP
		| (int)ECollisionLayer::ENVIRONMENT
		| (int)ECollisionLayer::ENVIRONMENT2
		| (int)ECollisionLayer::NATURE,

		ENVIRONMENT
		= (int)ECollisionLayer::PLAYER
		| (int)ECollisionLayer::ITEM
		| (int)ECollisionLayer::BOSS
		| (int)ECollisionLayer::ALLY
		| (int)ECollisionLayer::MONSTER
		| (int)ECollisionLayer::ALLY_WEAPON
		| (int)ECollisionLayer::PLAYER_WEAPON
		| (int)ECollisionLayer::MONSTER_WEAPON
		| (int)ECollisionLayer::ALLY_PROJECTILE
		| (int)ECollisionLayer::PLAYER_PROJECTILE
		| (int)ECollisionLayer::RIDING
		| (int)ECollisionLayer::MONSTER_PROJECTILE
		| (int)ECollisionLayer::BOWL
		| (int)ECollisionLayer::BOWLINGPIN,

		ENVIRONMENT2
		= (int)ECollisionLayer::PLAYER
		| (int)ECollisionLayer::ITEM
		| (int)ECollisionLayer::BOSS
		| (int)ECollisionLayer::ALLY
		| (int)ECollisionLayer::MONSTER
		| (int)ECollisionLayer::ALLY_WEAPON
		| (int)ECollisionLayer::PLAYER_WEAPON
		| (int)ECollisionLayer::MONSTER_WEAPON
		| (int)ECollisionLayer::ALLY_PROJECTILE
		| (int)ECollisionLayer::PLAYER_PROJECTILE
		| (int)ECollisionLayer::RIDING
		| (int)ECollisionLayer::MONSTER_PROJECTILE
		| (int)ECollisionLayer::BOWL
		| (int)ECollisionLayer::BOWLINGPIN,

		BUILDING
		= (int)ECollisionLayer::PLAYER
		| (int)ECollisionLayer::BOSS
		| (int)ECollisionLayer::MONSTER
		| (int)ECollisionLayer::ALLY_WEAPON
		| (int)ECollisionLayer::PLAYER_WEAPON
		| (int)ECollisionLayer::MONSTER_WEAPON
		| (int)ECollisionLayer::ALLY_PROJECTILE
		| (int)ECollisionLayer::PLAYER_PROJECTILE
		| (int)ECollisionLayer::MONSTER_PROJECTILE
		| (int)ECollisionLayer::INTERACTION
		| (int)ECollisionLayer::RIDING
		| (int)ECollisionLayer::BASE_CAMP
		| (int)ECollisionLayer::BOWL
		| (int)ECollisionLayer::BOWLINGPIN,

		NATURE
		= (int)ECollisionLayer::PLAYER
		| (int)ECollisionLayer::ITEM
		| (int)ECollisionLayer::BOSS
		| (int)ECollisionLayer::ALLY
		| (int)ECollisionLayer::MONSTER
		| (int)ECollisionLayer::ALLY_WEAPON
		| (int)ECollisionLayer::PLAYER_WEAPON
		| (int)ECollisionLayer::MONSTER_WEAPON
		| (int)ECollisionLayer::ALLY_PROJECTILE
		| (int)ECollisionLayer::PLAYER_PROJECTILE
		| (int)ECollisionLayer::MONSTER_PROJECTILE
		| (int)ECollisionLayer::RIDING
		| (int)ECollisionLayer::BASE_CAMP,

		INTERACTION
		= (int)ECollisionLayer::BUILDING
		| (int)ECollisionLayer::ITEM
		| (int)ECollisionLayer::RIDING
		| (int)ECollisionLayer::ALLY
		| (int)ECollisionLayer::NPC,

		BASE_CAMP
		= (int)ECollisionLayer::NATURE
		| (int)ECollisionLayer::ITEM
		| (int)ECollisionLayer::BUILDING
		| (int)ECollisionLayer::PLAYER,

		NPC
		= (int)ECollisionLayer::PLAYER
		| (int)ECollisionLayer::INTERACTION,

		RIDING
		= (int)ECollisionLayer::ENVIRONMENT
		| (int)ECollisionLayer::ENVIRONMENT2
		| (int)ECollisionLayer::BUILDING
		| (int)ECollisionLayer::INTERACTION
		| (int)ECollisionLayer::NATURE,

		BOWL
		= (int)ECollisionLayer::BUILDING
		| (int)ECollisionLayer::ENVIRONMENT
		| (int)ECollisionLayer::ENVIRONMENT2
		| (int)ECollisionLayer::INTERACTION
		| (int)ECollisionLayer::NATURE
		| (int)ECollisionLayer::BOWLINGPIN
		| (int)ECollisionLayer::BOWLINGTRIGGER
		| (int)ECollisionLayer::BOWLING_FLOOR,

		BOWLINGPIN
		= (int)ECollisionLayer::BUILDING
		| (int)ECollisionLayer::ENVIRONMENT
		| (int)ECollisionLayer::ENVIRONMENT2
		| (int)ECollisionLayer::INTERACTION
		| (int)ECollisionLayer::NATURE
		| (int)ECollisionLayer::BOWL
		| (int)ECollisionLayer::BOWLINGPIN
		| (int)ECollisionLayer::BOWLINGTRIGGER
		| (int)ECollisionLayer::BOWLING_FLOOR,

		MINIGAME_GACHA
		= (int)ECollisionLayer::PLAYER,

		BOWLINGTRIGGER
		= (int)ECollisionLayer::BOWL,

		BOWLING_FLOOR
		= (int)ECollisionLayer::BOWL
		| (int)ECollisionLayer::BOWLINGPIN,

		ENUM_END
	};

	/********************
		Input Enum
	********************/
	enum class EWheelState
	{
		SCROLL_DOWN,
		SCROLL_UP,
		ENUM_END
	};

	enum class EMouseState
	{
		DOWN,
		PRESSED,
		UP,
		RELEASED,
		ENUM_END
	};

	enum class EKeyState
	{
		DOWN,
		PRESSED,
		UP,
		RELEASED,
		ENUM_END
	};

	enum class EMouseActionType
	{
		LEFT_BUTTON,
		RIGHT_BUTTON,
		SCROLL_WHEEL_BUTTON,
		ENUM_END
	};

	enum class EMouseAxisType
	{
		X_AXIS,
		Y_AXIS,
		Z_AXIS,
		ENUM_END
	};

	/********************
		Map Enum
	********************/
	enum class EMapMeshType
	{
		GRID,
		ENVMESH,
		ENVCOLLISION,
		TREE,
		ROCK,
		WOOD,
		STONE,
		PAL,
		ARCHITECTURE,
		LIGHT,
		CAMERA,
		ENUM_END
	};
}
