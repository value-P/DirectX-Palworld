#include "pch.h"
#include "OctreeNode.h"

// manager
#include "TreeManager.h"
#include "World.h"
#include "Level.h"

// gameobject
#include "Monster.h"
#include "Tree.h"
#include "Rock.h"

#include "BerryGoat.h"
#include "Boar.h"
#include "ChickenPal.h"
#include "DreamDemon.h"
#include "ElecPanda.h"
#include "GrassMammoth.h"
#include "HedgeHog.h"
#include "HedgeHogIce.h"
#include "Kitsunebi.h"
#include "PinkCat.h"
#include "Penguin.h"
#include "SheepBall.h"
#include "WoolFox.h"

// utility
#include "StringUtility.h"

std::shared_ptr<COctreeNode> Client::COctreeNode::Create(int32 _iOctreeIndex, uint32 _iNodeDepth, const Vector3& _vCenter, const Vector3& _vExtents)
{
	const shared_ptr<COctreeNode> spOctreeNode = make_shared<COctreeNode>();
	ENSUREF(spOctreeNode->Initialize(_iOctreeIndex, _iNodeDepth, _vCenter, _vExtents), L"Failed to initialize octree node.");
	return spOctreeNode;
}

HRESULT Client::COctreeNode::Initialize(int32 _iTreeIndex, uint32 _iNodeDepth, const Vector3& _vCenter, const Vector3& _vExtents)
{
	if (FAILED(CTreeNode::Initialize(_iTreeIndex, _iNodeDepth, _vCenter, _vExtents))) { return E_FAIL; }

	// 트리 매니저
	m_wpTreeManager = CTreeManager::GetInstance();
	m_iNodeIndex = m_wpTreeManager.lock()->AddOctreeChildNode(_iTreeIndex, static_pointer_cast<COctreeNode>(shared_from_this()));
	m_iNodeDepth = _iNodeDepth;
	m_tDxAABB = BoundingBox(_vCenter, _vExtents);
	return S_OK;
}

void Client::COctreeNode::Release()
{
	/*
	for (auto& spHabitat : m_vecHabitats)
	{
		spHabitat = nullptr;
	}
	m_vecHabitats.clear();
	*/

	CTreeNode::Release();
}

HRESULT Client::COctreeNode::CreateOctreeChildNodes(uint32 _iMaxNodeDepth, int32 _iOctreeIndex)
{
	// 기저 조건입니다.
	if (m_iNodeDepth == _iMaxNodeDepth)
	{
		shared_ptr<CTreeNode> spRootNode = m_wpTreeManager.lock()->GetOctreeRootNode(_iOctreeIndex);
		Vector3 vRootNodeCenter = spRootNode->GetCenter();
		Vector3 vRootNodeExtents = spRootNode->GetExtents();

		float fX = vRootNodeCenter.x - vRootNodeExtents.x;
		float fY = vRootNodeCenter.y - vRootNodeExtents.y;
		float fZ = vRootNodeCenter.z - vRootNodeExtents.z;

		Vector3 vCenter = m_tDxAABB.Center;
		Vector3 vExtents = m_tDxAABB.Extents;

		int32 iIndexX = static_cast<int32>(vCenter.x - fX) / (static_cast<int32>(vExtents.x * 2.0f));
		int32 iIndexY = static_cast<int32>(vCenter.y - fY) / (static_cast<int32>(vExtents.y * 2.0f));
		int32 iIndexZ = static_cast<int32>(vCenter.z - fZ) / (static_cast<int32>(vExtents.z * 2.0f));

		m_tpOctreeIndex = { iIndexX, iIndexY, iIndexZ };
		m_bLeaf = true;

		m_wpTreeManager.lock()->SetOctreeLeafNode(_iOctreeIndex, { iIndexX, iIndexY, iIndexZ }, static_pointer_cast<COctreeNode>(shared_from_this()));
		return S_OK;
	}

	for (int32 iOuterIndex = -1; iOuterIndex < 2; iOuterIndex += 2)
	{
		for (int32 iMiddleIndex = -1; iMiddleIndex < 2; iMiddleIndex += 2)
		{
			for (int32 iInnerIndex = -1; iInnerIndex < 2; iInnerIndex += 2)
			{
				Vector3 vExtents = m_tDxAABB.Extents;
				vExtents.x *= 0.5f;
				vExtents.y *= 0.5f;
				vExtents.z *= 0.5f;

				Vector3 vCenter = m_tDxAABB.Center;
				vCenter.x += iInnerIndex * vExtents.x;
				vCenter.y += iMiddleIndex * vExtents.y;
				vCenter.z += iOuterIndex * vExtents.z;

				shared_ptr<COctreeNode> spChildNode = COctreeNode::Create(_iOctreeIndex, m_iNodeDepth + 1, vCenter, vExtents);
				m_vecChildNodes.push_back(spChildNode);
			}
		}
	}

	for (auto& spChildNode : m_vecChildNodes)
	{
		static_pointer_cast<COctreeNode>(spChildNode)->CreateOctreeChildNodes(_iMaxNodeDepth, _iOctreeIndex);
	}

	return S_OK;
}

std::shared_ptr<COctreeNode> Client::COctreeNode::LoadJsonAndAllocGameObject()
{
	// 게임 오브젝트가 이미 할당된 경우
	if (m_bGameObjectAlloced.load()) { return nullptr; }

	// json에서 데이터를 불러옵니다.
	auto& [iX, iY, iZ] = m_tpOctreeIndex;
	m_wstrGameObjectJsonFileName += to_wstring(iX);
	m_wstrGameObjectJsonFileName += to_wstring(iY);
	m_wstrGameObjectJsonFileName += to_wstring(iZ);

	ifstream isJsonpath(L"../../Resource/Models/Dat/MapMesh/Prototype/Octree8/" + m_wstrGameObjectJsonFileName + L".json");
	if (!isJsonpath.is_open()) { throw runtime_error("Failed to open json file."); }

	string strJsonfile((istreambuf_iterator<char>(isJsonpath)), (istreambuf_iterator<char>()));

	rapidjson::Document doc;
	doc.Parse(strJsonfile.c_str());

	rapidjson::Value& bEmpty = doc["Is Empty?"];
	bool bFileEmpty = bEmpty.GetBool();

	if (!bFileEmpty)
	{
		rapidjson::Value& iTreeCount = doc["Tree Count"];
		rapidjson::Value& iRockCount = doc["Rock Count"];
		rapidjson::Value& iPalCount = doc["Pal Count"];

		int32 iNumTreeCount = iTreeCount.GetInt();
		int32 iNumRockCount = iRockCount.GetInt();
		int32 iNumPalCount = iPalCount.GetInt();

		
		// 나무를 생성합니다.
		if (0 != iNumTreeCount)
		{
			
			rapidjson::Value& arrTree = doc["Trees Information"];

			FTreeCache tTreeCache{};
			tTreeCache.m_iHabitatType = arrTree["Habitat Type"].GetInt();
			tTreeCache.m_iNatureType = arrTree["Nature Type"].GetInt();

			int32 iTotalTreeCount = arrTree["Total Number of Trees"].GetInt();
			rapidjson::Value& arrIndividualInfo = arrTree["Individual Information"];

			int32 iIndex = -1;
			for (auto iter = arrIndividualInfo.MemberBegin(); iter != arrIndividualInfo.MemberEnd(); ++iter)
			{
				iIndex++;

				tTreeCache.m_vScale.x = iter->value[0]["Scale"][0].GetFloat();
				tTreeCache.m_vScale.y = iter->value[0]["Scale"][1].GetFloat();
				tTreeCache.m_vScale.z = iter->value[0]["Scale"][2].GetFloat();

				tTreeCache.m_vRotate.x = iter->value[1]["Rotation"][0].GetFloat();
				tTreeCache.m_vRotate.y = iter->value[1]["Rotation"][1].GetFloat();
				tTreeCache.m_vRotate.z = iter->value[1]["Rotation"][2].GetFloat();

				tTreeCache.m_vPosition.x = iter->value[2]["Translation"][0].GetFloat();
				tTreeCache.m_vPosition.y = iter->value[2]["Translation"][1].GetFloat();
				tTreeCache.m_vPosition.z = iter->value[2]["Translation"][2].GetFloat();

				tTreeCache.m_vCenter.x = iter->value[3]["Center"][0].GetFloat();
				tTreeCache.m_vCenter.y = iter->value[3]["Center"][1].GetFloat();
				tTreeCache.m_vCenter.z = iter->value[3]["Center"][2].GetFloat();

				tTreeCache.m_vExtents.x = iter->value[4]["Extents"][0].GetFloat();
				tTreeCache.m_vExtents.y = iter->value[4]["Extents"][1].GetFloat();
				tTreeCache.m_vExtents.z = iter->value[4]["Extents"][2].GetFloat();

				get<0>(tTreeCache.m_tpOctreeNodeIndex) = iter->value[5]["Included Octree Node Index"][0].GetInt();
				get<1>(tTreeCache.m_tpOctreeNodeIndex) = iter->value[5]["Included Octree Node Index"][1].GetInt();
				get<2>(tTreeCache.m_tpOctreeNodeIndex) = iter->value[5]["Included Octree Node Index"][2].GetInt();

				tTreeCache.m_wstrModelFilePath = CStringUtility::ToWideString(iter->value[6]["Model File Path"].GetString());
				ENSUREF(CreateTrees(tTreeCache), L"Failed to initialize tree.");
			}
		}

		// 바위를 생성합니다.
		if (0 != iNumRockCount)
		{
			rapidjson::Value& arrRock = doc["Rocks Information"];

			FRockCache tRockCache{};
			tRockCache.m_iHabitatType = arrRock["Habitat Type"].GetInt();
			tRockCache.m_iNatureType = arrRock["Nature Type"].GetInt();

			int32 iTotalRockCount = arrRock["Total Number of Rocks"].GetInt();
			rapidjson::Value& arrIndividualInfo = arrRock["Individual Information"];

			int32 iIndex = -1;
			for (auto iter = arrIndividualInfo.MemberBegin(); iter != arrIndividualInfo.MemberEnd(); ++iter)
			{
				iIndex++;

				tRockCache.m_vScale.x =  iter->value[0]["Scale"][0].GetFloat();
				tRockCache.m_vScale.y = iter->value[0]["Scale"][1].GetFloat();
				tRockCache.m_vScale.z = iter->value[0]["Scale"][2].GetFloat();

				tRockCache.m_vRotate.x = iter->value[1]["Rotation"][0].GetFloat();
				tRockCache.m_vRotate.y = iter->value[1]["Rotation"][1].GetFloat();
				tRockCache.m_vRotate.z = iter->value[1]["Rotation"][2].GetFloat();

				tRockCache.m_vPosition.x = iter->value[2]["Translation"][0].GetFloat();
				tRockCache.m_vPosition.y = iter->value[2]["Translation"][1].GetFloat();
				tRockCache.m_vPosition.z = iter->value[2]["Translation"][2].GetFloat();

				tRockCache.m_vCenter.x = iter->value[3]["Center"][0].GetFloat();
				tRockCache.m_vCenter.y = iter->value[3]["Center"][1].GetFloat();
				tRockCache.m_vCenter.z = iter->value[3]["Center"][2].GetFloat();

				tRockCache.m_fRadius = iter->value[4]["Radius"].GetFloat();

				get<0>(tRockCache.m_tpOctreeNodeIndex) = iter->value[5]["Included Octree Node Index"][0].GetInt();
				get<1>(tRockCache.m_tpOctreeNodeIndex) = iter->value[5]["Included Octree Node Index"][1].GetInt();
				get<2>(tRockCache.m_tpOctreeNodeIndex) = iter->value[5]["Included Octree Node Index"][2].GetInt();

				tRockCache.m_wstrModelFilePath = CStringUtility::ToWideString(iter->value[6]["Model File Path"].GetString());
				ENSUREF(CreateRocks(tRockCache), L"Failed to initialize rock.");
			}
			
		}
		
		if (0 != iNumPalCount)
		{
			rapidjson::Value& arrPal = doc["Pals Information"];

			FPalCache tPalCache{};

			int32 iTotalPalCount = arrPal["Total Number of Pals"].GetInt();
			rapidjson::Value& arrIndividualInfo = arrPal["Individual Information"];

			int32 iIndex = -1;
			for (auto iter = arrIndividualInfo.MemberBegin(); iter != arrIndividualInfo.MemberEnd(); ++iter)
			{
				iIndex++;

				tPalCache.m_vPosition.x = iter->value[0]["Translation"][0].GetFloat();
				tPalCache.m_vPosition.y = iter->value[0]["Translation"][1].GetFloat() + 1.0f;
				tPalCache.m_vPosition.z = iter->value[0]["Translation"][2].GetFloat();

				tPalCache.m_eMonsterType = static_cast<EMonsterType>(iter->value[1]["Monster Type"].GetUint());

				if (tPalCache.m_eMonsterType == EMonsterType::HEDGE_HOG_ICE)
				{
					tPalCache.m_vPosition.y += 5.f;
				}

				ENSUREF(CreatePals(tPalCache), L"Failed to initialize rock.");
			}
		}
		CHECKF(true, L"Save Completed");
	}

	m_bGameObjectAlloced.store(true);

	return static_pointer_cast<COctreeNode>(shared_from_this());
}

HRESULT Client::COctreeNode::ActivateGameObject()
{
	for (const auto& pair : m_umapStaticGameObjects)
	{
		pair.second->SetActive(true);
	}

	for (const auto& pair : m_umapDynamicGameObjects)
	{
		pair.second->SetActive(true);
	}

	return S_OK;
}

HRESULT Client::COctreeNode::DeactivateGameObject()
{
	for (const auto& pair : m_umapStaticGameObjects)
	{
		pair.second->SetActive(true);
	}

	for (const auto& pair : m_umapDynamicGameObjects)
	{
		pair.second->SetActive(true);
	}

	return S_OK;
}

HRESULT Client::COctreeNode::CreateRocks(const FRockCache& _tRockCache)
{
	// gameobject desc
	FRockDesc tRockDesc;
	tRockDesc.m_wstrGameObjectName = L"Rock";
	tRockDesc.m_eMobilityType = EMobilityType::STATIC;
	tRockDesc.m_wstrModelFilePath = _tRockCache.m_wstrModelFilePath;
	tRockDesc.m_vInitScale = _tRockCache.m_vScale;
	tRockDesc.m_vInitRotate = _tRockCache.m_vRotate;
	tRockDesc.m_vInitPosition = _tRockCache.m_vPosition;

	// rock desc
	tRockDesc.m_fRadius = _tRockCache.m_fRadius;
	tRockDesc.m_vCenter = _tRockCache.m_vCenter;

	shared_ptr<CRock> spRock = CRock::Create(make_shared<FRockDesc>(tRockDesc));

	// m_vecHabitats.emplace_back(spRock);
	m_wpCurLevel.lock()->AddGameObject(spRock);

	// 이 노드 안에 있는 정적 오브젝트로 추가합니다.
	return AddGameObjectToNode(spRock);
}

HRESULT Client::COctreeNode::CreateTrees(const FTreeCache& _tTreeCache)
{
	// gameobject desc
	FTreeDesc tTreeDesc;
	tTreeDesc.m_wstrGameObjectName = L"Tree";
	tTreeDesc.m_eMobilityType = EMobilityType::STATIC;
	tTreeDesc.m_wstrModelFilePath = _tTreeCache.m_wstrModelFilePath;
	tTreeDesc.m_vInitScale = _tTreeCache.m_vScale;
	tTreeDesc.m_vInitRotate = _tTreeCache.m_vRotate;
	tTreeDesc.m_vInitPosition = _tTreeCache.m_vPosition;

	// tree desc
	tTreeDesc.m_vExtents = _tTreeCache.m_vExtents;
	tTreeDesc.m_vCenter = _tTreeCache.m_vCenter;

	shared_ptr<CTree> spTree = CTree::Create(make_shared<FTreeDesc>(tTreeDesc));
	m_wpCurLevel.lock()->AddGameObject(spTree);

	// 이 노드 안에 있는 정적 오브젝트로 추가합니다.
	return AddGameObjectToNode(spTree);
}

HRESULT Client::COctreeNode::CreatePals(const FPalCache& _tPalCache)
{
	switch (_tPalCache.m_eMonsterType)
	{
	case Client::EMonsterType::BERRY_GOAT:
	{
		// gameobject desc
		FBerryGoatDesc tBerryGoatDesc;
		tBerryGoatDesc.m_wstrGameObjectName = L"BerryGoat";
		tBerryGoatDesc.m_eMobilityType		= EMobilityType::DYNAMIC;
		tBerryGoatDesc.m_vInitPosition		= _tPalCache.m_vPosition;
		tBerryGoatDesc.m_iOctreeIndex		= 0;
		tBerryGoatDesc.m_tpOctreeNodeIndex	= m_tpOctreeIndex;

		// character desc

		// creature desc
		tBerryGoatDesc.m_eCreatureType = ECreatureType::PAL;

		// monster desc
		tBerryGoatDesc.m_eMonsterType = EMonsterType::BERRY_GOAT;
		tBerryGoatDesc.m_eMonsterPersonalityType = EMonsterPersonalityType::STUPID;
		tBerryGoatDesc.m_eMonsterAIType = EMonsterAIType::WILD_PAL;

		shared_ptr<CBerryGoat> spBerryGoat = CBerryGoat::Create(make_shared<FBerryGoatDesc>(tBerryGoatDesc));
		spBerryGoat->SetJump(true);
		m_wpCurLevel.lock()->AddGameObject(spBerryGoat);

		// 이 노드 안에 있는 동적 오브젝트로 추가합니다.
		ENSURE(AddGameObjectToNode(spBerryGoat));
	}
	break;

	case Client::EMonsterType::BOAR:
	{
		// gameobject desc
		FBoarDesc tBoarDesc;
		tBoarDesc.m_wstrGameObjectName	= L"Boar";
		tBoarDesc.m_eMobilityType		= EMobilityType::DYNAMIC;
		tBoarDesc.m_vInitPosition		= _tPalCache.m_vPosition;
		tBoarDesc.m_iOctreeIndex		= 0;
		tBoarDesc.m_tpOctreeNodeIndex	= m_tpOctreeIndex;

		// character desc

		// creature desc
		tBoarDesc.m_eCreatureType = ECreatureType::PAL;

		// monster desc
		tBoarDesc.m_eMonsterType = EMonsterType::BOAR;
		tBoarDesc.m_eMonsterPersonalityType = EMonsterPersonalityType::AGGRESSIVE;
		tBoarDesc.m_eMonsterAIType = EMonsterAIType::WILD_PAL;

		shared_ptr<CBoar> spBoar = CBoar::Create(make_shared<FBoarDesc>(tBoarDesc));
		spBoar->SetJump(true);

		// m_vecPals.emplace_back(spBoar);
		m_wpCurLevel.lock()->AddGameObject(spBoar);

		// 이 노드 안에 있는 동적 오브젝트로 추가합니다.
		ENSURE(AddGameObjectToNode(spBoar));
	}
	break;

	case Client::EMonsterType::DREAM_DEMON:
	{
		// gameobject desc
		FDreamDemonDesc tDreamDemonDesc;
		tDreamDemonDesc.m_wstrGameObjectName	= L"DreamDemon";
		tDreamDemonDesc.m_eMobilityType			= EMobilityType::DYNAMIC;
		tDreamDemonDesc.m_vInitPosition			= _tPalCache.m_vPosition;
		tDreamDemonDesc.m_iOctreeIndex			= 0;
		tDreamDemonDesc.m_tpOctreeNodeIndex		= m_tpOctreeIndex;

		// character desc

		// creature desc
		tDreamDemonDesc.m_eCreatureType = ECreatureType::PAL;

		// monster desc
		tDreamDemonDesc.m_eMonsterType = EMonsterType::DREAM_DEMON;
		tDreamDemonDesc.m_eMonsterPersonalityType = EMonsterPersonalityType::STUPID;
		tDreamDemonDesc.m_eMonsterAIType = EMonsterAIType::WILD_PAL;

		shared_ptr<CDreamDemon> spDreamDemon = CDreamDemon::Create(make_shared<FDreamDemonDesc>(tDreamDemonDesc));
		spDreamDemon->SetJump(true);

		// m_vecPals.emplace_back(spDreamDemon);
		m_wpCurLevel.lock()->AddGameObject(spDreamDemon);

		// 이 노드 안에 있는 동적 오브젝트로 추가합니다.
		ENSURE(AddGameObjectToNode(spDreamDemon));
	}
	break;

	case Client::EMonsterType::GRASS_MAMMOTH:
	{
		// gameobject desc
		FGrassMammothDesc tGrassMammothDesc;
		tGrassMammothDesc.m_wstrGameObjectName	= L"GrassMammoth";
		tGrassMammothDesc.m_eMobilityType		= EMobilityType::DYNAMIC;
		tGrassMammothDesc.m_vInitPosition		= _tPalCache.m_vPosition;
		tGrassMammothDesc.m_iOctreeIndex		= 0;
		tGrassMammothDesc.m_tpOctreeNodeIndex	= m_tpOctreeIndex;

		// character desc

		// creature desc
		tGrassMammothDesc.m_eCreatureType = ECreatureType::PAL;

		// monster desc
		tGrassMammothDesc.m_eMonsterType = EMonsterType::GRASS_MAMMOTH;
		tGrassMammothDesc.m_eMonsterPersonalityType = EMonsterPersonalityType::STUPID;
		tGrassMammothDesc.m_eMonsterAIType = EMonsterAIType::WILD_PAL;

		shared_ptr<CGrassMammoth> spGrassMammoth = CGrassMammoth::Create(make_shared<FGrassMammothDesc>(tGrassMammothDesc));
		spGrassMammoth->SetJump(true);

		// m_vecPals.emplace_back(spGrassMammoth);
		m_wpCurLevel.lock()->AddGameObject(spGrassMammoth);

		// 이 노드 안에 있는 동적 오브젝트로 추가합니다.
		ENSURE(AddGameObjectToNode(spGrassMammoth));
	}
	break;

	case Client::EMonsterType::HEDGE_HOG:
	{
		// gameobject desc
		FHedgeHogDesc tHedgeHogDesc;
		tHedgeHogDesc.m_wstrGameObjectName	= L"HedgeHog";
		tHedgeHogDesc.m_eMobilityType		= EMobilityType::DYNAMIC;
		tHedgeHogDesc.m_vInitPosition		= _tPalCache.m_vPosition;
		tHedgeHogDesc.m_iOctreeIndex		= 0;
		tHedgeHogDesc.m_tpOctreeNodeIndex	= m_tpOctreeIndex;

		// character desc

		// creature desc
		tHedgeHogDesc.m_eCreatureType = ECreatureType::PAL;

		// monster desc
		tHedgeHogDesc.m_eMonsterType = EMonsterType::HEDGE_HOG;
		tHedgeHogDesc.m_eMonsterPersonalityType = EMonsterPersonalityType::STUPID;
		tHedgeHogDesc.m_eMonsterAIType = EMonsterAIType::WILD_PAL;

		shared_ptr<CHedgeHog> spHedgeHog = CHedgeHog::Create(make_shared<FHedgeHogDesc>(tHedgeHogDesc));
		spHedgeHog->SetJump(true);

		// m_vecPals.emplace_back(spHedgeHog);
		m_wpCurLevel.lock()->AddGameObject(spHedgeHog);

		// 이 노드 안에 있는 동적 오브젝트로 추가합니다.
		ENSURE(AddGameObjectToNode(spHedgeHog));
	}
	break;

	case Client::EMonsterType::HEDGE_HOG_ICE:
	{
		// gameobject desc
		FHedgeHogIceDesc tHedgeHogIceDesc;
		tHedgeHogIceDesc.m_wstrGameObjectName	= L"HedgeHogIce";
		tHedgeHogIceDesc.m_eMobilityType		= EMobilityType::DYNAMIC;
		tHedgeHogIceDesc.m_vInitPosition		= _tPalCache.m_vPosition;
		tHedgeHogIceDesc.m_iOctreeIndex			= 0;
		tHedgeHogIceDesc.m_tpOctreeNodeIndex	= m_tpOctreeIndex;

		// character desc

		// creature desc
		tHedgeHogIceDesc.m_eCreatureType = ECreatureType::PAL;

		// monster desc
		tHedgeHogIceDesc.m_eMonsterType = EMonsterType::HEDGE_HOG_ICE;
		tHedgeHogIceDesc.m_eMonsterPersonalityType = EMonsterPersonalityType::PEACEFUL;
		tHedgeHogIceDesc.m_eMonsterAIType = EMonsterAIType::WILD_PAL;

		shared_ptr<CHedgeHogIce> spHedgeHogIce = CHedgeHogIce::Create(make_shared<FHedgeHogIceDesc>(tHedgeHogIceDesc));
		spHedgeHogIce->SetJump(true);

		// m_vecPals.emplace_back(spHedgeHogIce);
		m_wpCurLevel.lock()->AddGameObject(spHedgeHogIce);

		// 이 노드 안에 있는 동적 오브젝트로 추가합니다.
		ENSURE(AddGameObjectToNode(spHedgeHogIce));
	}
	break;

	case Client::EMonsterType::KITSUNEBI:
	{
		// gameobject desc
		FKitsunebiDesc tKitsunebiDesc;
		tKitsunebiDesc.m_wstrGameObjectName = L"Kitsunebi";
		tKitsunebiDesc.m_eMobilityType		= EMobilityType::DYNAMIC;
		tKitsunebiDesc.m_vInitPosition		= _tPalCache.m_vPosition;
		tKitsunebiDesc.m_iOctreeIndex		= 0;
		tKitsunebiDesc.m_tpOctreeNodeIndex	= m_tpOctreeIndex;

		// creature desc
		tKitsunebiDesc.m_eCreatureType = ECreatureType::PAL;

		// monster desc
		tKitsunebiDesc.m_eMonsterType = EMonsterType::KITSUNEBI;
		tKitsunebiDesc.m_eMonsterPersonalityType = EMonsterPersonalityType::AGGRESSIVE;
		tKitsunebiDesc.m_eMonsterAIType = EMonsterAIType::WILD_PAL;

		shared_ptr<CKitsunebi> spKitsunebi = CKitsunebi::Create(make_shared<FKitsunebiDesc>(tKitsunebiDesc));
		spKitsunebi->SetJump(true);

		// m_vecPals.emplace_back(spKitsunebi);
		m_wpCurLevel.lock()->AddGameObject(spKitsunebi);

		// 이 노드 안에 있는 동적 오브젝트로 추가합니다.
		ENSURE(AddGameObjectToNode(spKitsunebi));
	}
	break;

	case Client::EMonsterType::PENGUIN:
	{
		// gameobject desc
		FPenguinDesc tPenguinDesc;
		tPenguinDesc.m_wstrGameObjectName	= L"Penguin";
		tPenguinDesc.m_eMobilityType		= EMobilityType::DYNAMIC;
		tPenguinDesc.m_vInitPosition		= _tPalCache.m_vPosition;
		tPenguinDesc.m_iOctreeIndex			= 0;
		tPenguinDesc.m_tpOctreeNodeIndex	= m_tpOctreeIndex;

		// character desc

		// creature desc
		tPenguinDesc.m_eCreatureType = ECreatureType::PAL;

		// monster desc
		tPenguinDesc.m_eMonsterType = EMonsterType::PENGUIN;
		tPenguinDesc.m_eMonsterPersonalityType = EMonsterPersonalityType::STUPID;
		tPenguinDesc.m_eMonsterAIType = EMonsterAIType::WILD_PAL;

		shared_ptr<CPenguin> spPenguin = CPenguin::Create(make_shared<FPenguinDesc>(tPenguinDesc));
		spPenguin->SetJump(true);

		// m_vecPals.emplace_back(spPenguin);
		m_wpCurLevel.lock()->AddGameObject(spPenguin);

		// 이 노드 안에 있는 동적 오브젝트로 추가합니다.
		ENSURE(AddGameObjectToNode(spPenguin));
	}
	break;

	case Client::EMonsterType::PINK_CAT:
	{
		// gameobject desc
		FPinkCatDesc tPinkCatDesc;
		tPinkCatDesc.m_wstrGameObjectName	= L"PinkCat";
		tPinkCatDesc.m_eMobilityType		= EMobilityType::DYNAMIC;
		tPinkCatDesc.m_vInitPosition		= _tPalCache.m_vPosition;
		tPinkCatDesc.m_iOctreeIndex			= 0;
		tPinkCatDesc.m_tpOctreeNodeIndex	= m_tpOctreeIndex;

		// character desc

		// creature desc
		tPinkCatDesc.m_eCreatureType = ECreatureType::PAL;

		// monster desc
		tPinkCatDesc.m_eMonsterType = EMonsterType::PINK_CAT;
		tPinkCatDesc.m_eMonsterPersonalityType = EMonsterPersonalityType::STUPID;
		tPinkCatDesc.m_eMonsterAIType = EMonsterAIType::WILD_PAL;

		shared_ptr<CPinkCat> spPinkCat = CPinkCat::Create(make_shared<FPinkCatDesc>(tPinkCatDesc));
		spPinkCat->SetJump(true);

		// m_vecPals.emplace_back(spPinkCat);
		m_wpCurLevel.lock()->AddGameObject(spPinkCat);

		// 이 노드 안에 있는 동적 오브젝트로 추가합니다.
		ENSURE(AddGameObjectToNode(spPinkCat));
	}
	break;

	case Client::EMonsterType::SHEEP_BALL:
	{
		// gameobject desc
		FSheepBallDesc tSheepBallDesc;
		tSheepBallDesc.m_wstrGameObjectName = L"SheepBall";
		tSheepBallDesc.m_eMobilityType		= EMobilityType::DYNAMIC;
		tSheepBallDesc.m_vInitPosition		= _tPalCache.m_vPosition;
		tSheepBallDesc.m_iOctreeIndex		= 0;
		tSheepBallDesc.m_tpOctreeNodeIndex	= m_tpOctreeIndex;

		// character desc

		// creature desc
		tSheepBallDesc.m_eCreatureType = ECreatureType::PAL;

		// monster desc
		tSheepBallDesc.m_eMonsterType = EMonsterType::SHEEP_BALL;
		tSheepBallDesc.m_eMonsterPersonalityType = EMonsterPersonalityType::STUPID;
		tSheepBallDesc.m_eMonsterAIType = EMonsterAIType::WILD_PAL;

		shared_ptr<CSheepBall> spSheepBall = CSheepBall::Create(make_shared<FSheepBallDesc>(tSheepBallDesc));
		spSheepBall->SetJump(true);

		// m_vecPals.emplace_back(spSheepBall);
		m_wpCurLevel.lock()->AddGameObject(spSheepBall);

		// 이 노드 안에 있는 동적 오브젝트로 추가합니다.
		ENSURE(AddGameObjectToNode(spSheepBall));
	}
	break;

	case Client::EMonsterType::WOOL_FOX:
	{
		// gameobject desc
		FWoolFoxDesc tWoolFoxDesc;
		tWoolFoxDesc.m_wstrGameObjectName	= L"WoolFox";
		tWoolFoxDesc.m_eMobilityType		= EMobilityType::DYNAMIC;
		tWoolFoxDesc.m_vInitPosition		= _tPalCache.m_vPosition;
		tWoolFoxDesc.m_iOctreeIndex			= 0;
		tWoolFoxDesc.m_tpOctreeNodeIndex	= m_tpOctreeIndex;

		// character desc

		// creature desc
		tWoolFoxDesc.m_eCreatureType = ECreatureType::PAL;

		// monster desc
		tWoolFoxDesc.m_eMonsterType = EMonsterType::WOOL_FOX;
		tWoolFoxDesc.m_eMonsterPersonalityType = EMonsterPersonalityType::PEACEFUL;
		tWoolFoxDesc.m_eMonsterAIType = EMonsterAIType::WILD_PAL;

		shared_ptr<CWoolFox> spWoolFox = CWoolFox::Create(make_shared<FWoolFoxDesc>(tWoolFoxDesc));
		spWoolFox->SetJump(true);

		// m_vecPals.emplace_back(spWoolFox);
		m_wpCurLevel.lock()->AddGameObject(spWoolFox);

		// 이 노드 안에 있는 동적 오브젝트로 추가합니다.
		ENSURE(AddGameObjectToNode(spWoolFox));
	}
	break;

	case Client::EMonsterType::CARBUNCLO:
	case Client::EMonsterType::CHICKEN_PAL:
	case Client::EMonsterType::ELEC_PANDA:
	case Client::EMonsterType::FENGYUNDEEPER:
	case Client::EMonsterType::JETDRAGON:
		CHECKF(false, L"Monster type is wrong.");
		break;

	default:
		CHECKF(false, L"Monster type is not designated.");
		break;
	}

	return S_OK;
}
