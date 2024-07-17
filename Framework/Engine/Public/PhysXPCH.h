#pragma once

// PhysX 라이브러리
#define PVD_HOST "127.0.0.1"
#include <PxPhysicsAPI.h>
using namespace physx;

#ifdef _DEBUG

#pragma comment(lib, "./debug/SimulationController_static_64.lib")
#pragma comment(lib, "./debug/SceneQuery_static_64.lib")
#pragma comment(lib, "./debug/PhysXVehicle2_static_64.lib")
#pragma comment(lib, "./debug/PhysXVehicle_static_64.lib")
#pragma comment(lib, "./debug/PhysXTask_static_64.lib")
#pragma comment(lib, "./debug/PhysXPvdSDK_static_64.lib")
#pragma comment(lib, "./debug/PhysXFoundation_64.lib")
#pragma comment(lib, "./debug/PhysXExtensions_static_64.lib")
#pragma comment(lib, "./debug/PhysXCooking_64.lib")
#pragma comment(lib, "./debug/PhysXCommon_64.lib")
#pragma comment(lib, "./debug/PhysXCharacterKinematic_static_64.lib")
#pragma comment(lib, "./debug/PhysX_64.lib")
#pragma comment(lib, "./debug/LowLevelDynamics_static_64.lib")
#pragma comment(lib, "./debug/LowLevelAABB_static_64.lib")
#pragma comment(lib, "./debug/LowLevel_static_64.lib")

#else

#pragma comment(lib, "./release/SimulationController_static_64.lib")
#pragma comment(lib, "./release/SceneQuery_static_64.lib")
#pragma comment(lib, "./release/PhysXVehicle2_static_64.lib")
#pragma comment(lib, "./release/PhysXVehicle_static_64.lib")
#pragma comment(lib, "./release/PhysXTask_static_64.lib")
#pragma comment(lib, "./release/PhysXPvdSDK_static_64.lib")
#pragma comment(lib, "./release/PhysXFoundation_64.lib")
#pragma comment(lib, "./release/PhysXExtensions_static_64.lib")
#pragma comment(lib, "./release/PhysXCooking_64.lib")
#pragma comment(lib, "./release/PhysXCommon_64.lib")
#pragma comment(lib, "./release/PhysXCharacterKinematic_static_64.lib")
#pragma comment(lib, "./release/PhysX_64.lib")
#pragma comment(lib, "./release/LowLevelDynamics_static_64.lib")
#pragma comment(lib, "./release/LowLevelAABB_static_64.lib")
#pragma comment(lib, "./release/LowLevel_static_64.lib")

#endif // DEBUG
