#pragma once

// Windows 라이브러리
#define NOMINMAX
#include <Windows.h>
#include <wrl.h>
using namespace Microsoft::WRL;

// imgui 라이브러리
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
// #include "ImSequencer.h"
// #include "ImGuizmo.h"

// PhysX 라이브러리
#define PVD_HOST "127.0.0.1"
#include <PxPhysicsAPI.h>
using namespace physx;

// SimpleMath
#include <SimpleMath.h>
using namespace DirectX::SimpleMath;

// 사용자 정의 라이브러리
#include "ToolAnimationDefine.h"
#include "ToolAnimationStruct.h"
using namespace ToolAnimation;