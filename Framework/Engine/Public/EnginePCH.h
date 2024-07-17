#pragma once
#pragma warning(disable:4251)
#pragma warning(disable:4275)

// Windows 라이브러리
#include <Windows.h>
#include <wrl.h>
using namespace Microsoft::WRL;

// Direct3D 라이브러리
#include <d3d11.h>
#include <d3d11_4.h>
#include <DirectXMath.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>
#include <d3dcompiler.h>

#define INITGUID
#include <d3d11shader.h>
#include <d3dx11effect.h>

#define DIRECTINPUT_VERSION	0x0800
#include <dinput.h>
#include <SimpleMath.h>
using namespace DirectX::SimpleMath;

#include <DDSTextureLoader.h>
#include <WICTextureLoader.h>
#include <ScreenGrab.h>

#include <PrimitiveBatch.h>
#include <VertexTypes.h>
#include <Effects.h>
#include <SpriteBatch.h>
#include <SpriteFont.h>
#include <DebugDraw.h>
using namespace DirectX;

// C++ 표준 라이브러리
#include <iostream>
#include <string>
#include <array>
#include <vector>
#include <list>
#include <stack>
#include <queue>
#include <set>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>
#include <numeric>
#include <utility>
#include <functional>
#include <memory>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <random>
#include <chrono>
#include <thread>
#include <mutex>
#include <future>
#include <functional>
#include <condition_variable>
#include <format>
#include <atlstr.h>
#include <any>
using namespace std;

// C 라이브러리
#include <cassert>
#define _USE_MATH_DEFINES // for C++
#include <cmath>
#include <ctime>
#include <cstdio>
#include <cstdarg>

// FMOD 라이브러리
#include "fmod.hpp"

#ifdef _DEBUG
#pragma comment(lib, "jsoncppd.lib")
#else
#pragma comment(lib, "jsoncpp.lib")
#endif

// JSONCPP 라이브러리
#include <forwards.h>
#include <json.h>

// RapidJSON
#include "document.h"
#include "writer.h"
#include "stringbuffer.h"
#include "prettywriter.h"

// 사용자 정의 헤더 파일
#include "EngineType.h"
#include "EngineDefine.h"
#include "EngineEnum.h"
#include "EngineStruct.h"
#include "EngineDesc.h"
using namespace Engine;
