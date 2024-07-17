#pragma once

// Windows 라이브러리
#define NOMINMAX
#include <Windows.h>
#include <wrl.h>
using namespace Microsoft::WRL;

//#ifdef _DEBUG
//#pragma comment(lib, "jsoncppd.lib")
//#else
//#pragma comment(lib, "jsoncpp.lib")
//#endif
//
//// JSONCPP 라이브러리
//#include <forwards.h>
//#include <json.h>

// RapidJSON
#include "document.h"
#include "writer.h"
#include "stringbuffer.h"
#include "prettywriter.h"

// PhysX 라이브러리
#define PVD_HOST "127.0.0.1"
#include <PxPhysicsAPI.h>
using namespace physx;

// 사용자 정의 라이브러리
#include "ClientDefine.h"
#include "ClientEnum.h"
#include "ClientStruct.h"
#include "ClientDataSheet.h"
#include "AnimationEnum.h"
using namespace Client;
