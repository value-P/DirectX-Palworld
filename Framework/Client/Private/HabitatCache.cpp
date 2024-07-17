#include "pch.h"
#include "HabitatCache.h"

#include "StringUtility.h"

std::shared_ptr<CHabitatCache> Client::CHabitatCache::Create(const std::wstring& _wstrJsonFileName)
{
	const std::shared_ptr<CHabitatCache> spHabitatCache = make_shared<CHabitatCache>();
	ENSUREF(spHabitatCache->LoadHabitatDataFromJson(_wstrJsonFileName), L"Failed to initialize habitat cache.");
	return spHabitatCache;
}

HRESULT Client::CHabitatCache::LoadHabitatDataFromJson(const std::wstring& _wstrJsonFileName)
{
	/*
	// 이번튼 누르면, MSGBox 뜨고, text창 안에 있는 글자 지우기
	// 1. JSON문서를 읽기 모드로 연다.
	ifstream isJsonpath(L"../../Resource/Models/Dat/MapMesh/Prototype/" + _wstrJsonFileName + L".json");
	if (!isJsonpath.is_open()) { throw runtime_error("failed to open json file"); }

	// 2. 문서 내용을 문자열로 변환.
	string strJsonfile((istreambuf_iterator<char>(isJsonpath)), (istreambuf_iterator<char>()));

	// 3. JSON문서의 문자열을 파싱해온다.
	rapidjson::Document doc;
	doc.Parse(strJsonfile.c_str());

	// 4. JSON문서로부터 데이터를 읽는다.
	rapidjson::Value& bGridMapListBoxEmpty			= doc["GridMapListBox is Empty"];
	rapidjson::Value& bTreeListBoxEmpty				= doc["TreeListBox is Empty"];
	rapidjson::Value& bRockListBoxEmpty				= doc["RockListBox is Empty"];
	rapidjson::Value& bBranchListBoxEmpty			= doc["BranchListBox is Empty"];
	rapidjson::Value& bStoneListBoxEmpty			= doc["StoneListBox is Empty"];
	rapidjson::Value& bPalListBoxEmpty				= doc["PalListBox is Empty"];
	rapidjson::Value& bArchitectureListBoxEmpty		= doc["ArchitectureListBox is Empty"];
	rapidjson::Value& bSpotLightListBoxEmpty		= doc["SpotLightListBox is Empty"];
	rapidjson::Value& bPointLightListBoxEmpty		= doc["PointLightListBox is Empty"];
	rapidjson::Value& bDirectionalLightListBoxEmpty = doc["DirectionalLightListBox is Empty"];
	rapidjson::Value& bCameraListBoxEmpty			= doc["CameraListBox is Empty"];

	bool bGridMapEmpty			= bGridMapListBoxEmpty.GetBool();
	bool bTreeEmpty				= bTreeListBoxEmpty.GetBool();
	bool bRockEmpty				= bRockListBoxEmpty.GetBool();
	bool bBranchEmpty			= bBranchListBoxEmpty.GetBool();
	bool bStoneEmpty			= bStoneListBoxEmpty.GetBool();
	bool bPalEmpty				= bPalListBoxEmpty.GetBool();
	bool bArchitectureEmpty		= bArchitectureListBoxEmpty.GetBool();
	bool bSpotLightEmpty		= bSpotLightListBoxEmpty.GetBool();
	bool bPointLightEmpty		= bPointLightListBoxEmpty.GetBool();
	bool bDirectionalLightEmpty = bDirectionalLightListBoxEmpty.GetBool();
	bool bCameraEmpty			= bCameraListBoxEmpty.GetBool();

	if (!bGridMapEmpty)
	{

	}

	if (!bTreeEmpty)
	{
		rapidjson::Value& arrTreeListBoxEmpty = doc["Trees Information"];

		FHabitatTreeOrRockCache tHabitatTree{};

		tHabitatTree.m_bActive				= arrTreeListBoxEmpty["Active"].GetBool();
		tHabitatTree.m_bRespawn				= arrTreeListBoxEmpty["Respawn"].GetBool();
		tHabitatTree.m_bMinable				= arrTreeListBoxEmpty["Minable"].GetBool();
		tHabitatTree.m_bIsTrigger			= arrTreeListBoxEmpty["IsTrigger"].GetBool();
		tHabitatTree.m_fRadius				= arrTreeListBoxEmpty["Radius"].GetFloat();
		tHabitatTree.m_fHalfHeight			= arrTreeListBoxEmpty["HalfHeight"].GetFloat();
		tHabitatTree.m_fCurHp				= arrTreeListBoxEmpty["CurHp"].GetFloat();
		tHabitatTree.m_fMaxHp				= arrTreeListBoxEmpty["MaxHp"].GetFloat();
		tHabitatTree.m_fDropItemPercentage	= arrTreeListBoxEmpty["Drop Item Percentage"].GetFloat();
		tHabitatTree.m_fDropItemEvent		= arrTreeListBoxEmpty["Drop Item Event"].GetFloat();
		tHabitatTree.m_fRespawnTime			= arrTreeListBoxEmpty["Respawn Time"].GetFloat();
		tHabitatTree.m_iTotalDropItemCount	= arrTreeListBoxEmpty["Total Drop Item Count"].GetUint();
		tHabitatTree.m_iMobilityType		= arrTreeListBoxEmpty["Mobility Type"].GetUint();
		tHabitatTree.m_iHabitatType			= arrTreeListBoxEmpty["Habitat Type"].GetUint();
		tHabitatTree.m_iColliderType		= arrTreeListBoxEmpty["Collider Type"].GetUint();

		tHabitatTree.m_wstrDropItemAName  = CStringUtility::ToWideString(arrTreeListBoxEmpty["Drop Item A Name"].GetString());
		tHabitatTree.m_wstrDropItemBName  = CStringUtility::ToWideString(arrTreeListBoxEmpty["Drop Item B Name"].GetString());
		tHabitatTree.m_wstrEffectFilePath = CStringUtility::ToWideString(arrTreeListBoxEmpty["Effect File Path"].GetString());

		uint32 iTreeCount = arrTreeListBoxEmpty["Total Number of Trees"].GetUint();
		rapidjson::Value& arrIndividualInfo = arrTreeListBoxEmpty["Individual Information"];

		for (uint32 iIndex = 0; iIndex < iTreeCount; iIndex++)
		{
			wstring wstrTreeID = to_wstring(iIndex);
			wstring wstrTreeName = L"Tree" + wstrTreeID;

			rapidjson::Value& arrChildDesc = arrIndividualInfo[CStringUtility::ToString(wstrTreeName).c_str()];

			tHabitatTree.m_wstrModelFilePath = CStringUtility::ToWideString(arrChildDesc[0]["Model File Path"].GetString());

			tHabitatTree.m_vScale.x = static_cast<float>(arrChildDesc[1]["Scale"][0].GetDouble());
			tHabitatTree.m_vScale.y = static_cast<float>(arrChildDesc[1]["Scale"][1].GetDouble());
			tHabitatTree.m_vScale.z = static_cast<float>(arrChildDesc[1]["Scale"][2].GetDouble());

			tHabitatTree.m_vRotation.x = static_cast<float>(arrChildDesc[2]["Rotation"][0].GetDouble());
			tHabitatTree.m_vRotation.y = static_cast<float>(arrChildDesc[2]["Rotation"][1].GetDouble());
			tHabitatTree.m_vRotation.z = static_cast<float>(arrChildDesc[2]["Rotation"][2].GetDouble());

			tHabitatTree.m_vTranslation.x = static_cast<float>(arrChildDesc[3]["Translation"][0].GetDouble());
			tHabitatTree.m_vTranslation.y = static_cast<float>(arrChildDesc[3]["Translation"][1].GetDouble());
			tHabitatTree.m_vTranslation.z = static_cast<float>(arrChildDesc[3]["Translation"][2].GetDouble());

			get<0>(tHabitatTree.m_tpOctreeNodeIndex) = arrChildDesc[4]["Included Octree Node Index"][0].GetUint();
			get<1>(tHabitatTree.m_tpOctreeNodeIndex) = arrChildDesc[4]["Included Octree Node Index"][1].GetUint();
			get<2>(tHabitatTree.m_tpOctreeNodeIndex) = arrChildDesc[4]["Included Octree Node Index"][2].GetUint();

			m_vecHabitatTreeOrRockCaches.push_back(tHabitatTree);
		}
	}

	if (!bRockEmpty)
	{
		rapidjson::Value& arrRockListBoxEmpty = doc["Rocks Information"];

		FHabitatTreeOrRockCache tHabitatRock{};

		tHabitatRock.m_bActive				= arrRockListBoxEmpty["Active"].GetBool();
		tHabitatRock.m_bRespawn				= arrRockListBoxEmpty["Respawn"].GetBool();
		tHabitatRock.m_bMinable				= arrRockListBoxEmpty["Minable"].GetBool();
		tHabitatRock.m_bIsTrigger			= arrRockListBoxEmpty["IsTrigger"].GetBool();
		tHabitatRock.m_fRadius				= arrRockListBoxEmpty["Radius"].GetFloat();
		tHabitatRock.m_fHalfHeight			= arrRockListBoxEmpty["HalfHeight"].GetFloat();
		tHabitatRock.m_fCurHp				= arrRockListBoxEmpty["CurHp"].GetFloat();
		tHabitatRock.m_fMaxHp				= arrRockListBoxEmpty["MaxHp"].GetFloat();
		tHabitatRock.m_fDropItemPercentage	= arrRockListBoxEmpty["Drop Item Percentage"].GetFloat();
		tHabitatRock.m_fDropItemEvent		= arrRockListBoxEmpty["Drop Item Event"].GetFloat();
		tHabitatRock.m_fRespawnTime			= arrRockListBoxEmpty["Respawn Time"].GetFloat();
		tHabitatRock.m_iTotalDropItemCount	= arrRockListBoxEmpty["Total Drop Item Count"].GetUint();
		tHabitatRock.m_iMobilityType		= arrRockListBoxEmpty["Mobility Type"].GetUint();
		tHabitatRock.m_iHabitatType			= arrRockListBoxEmpty["Habitat Type"].GetUint();
		tHabitatRock.m_iColliderType		= arrRockListBoxEmpty["Collider Type"].GetUint();

		tHabitatRock.m_wstrDropItemAName  = CStringUtility::ToWideString(arrRockListBoxEmpty["Drop Item A Name"].GetString());
		tHabitatRock.m_wstrDropItemBName  = CStringUtility::ToWideString(arrRockListBoxEmpty["Drop Item B Name"].GetString());
		tHabitatRock.m_wstrEffectFilePath = CStringUtility::ToWideString(arrRockListBoxEmpty["Effect File Path"].GetString());


		uint32 iRockCount = arrRockListBoxEmpty["Total Number of Rocks"].GetUint();
		rapidjson::Value& arrIndividualInfo = arrRockListBoxEmpty["Individual Information"];

		for (uint32 iIndex = 0; iIndex < iRockCount; iIndex++)
		{
			wstring wstrRockID = to_wstring(iIndex);
			wstring wstrRockName = L"Rock" + wstrRockID;

			rapidjson::Value& arrChildDesc = arrIndividualInfo[CStringUtility::ToString(wstrRockName).c_str()];

			tHabitatRock.m_wstrModelFilePath = CStringUtility::ToWideString(arrChildDesc[0]["Model File Path"].GetString());

			tHabitatRock.m_vScale.x = static_cast<float>(arrChildDesc[1]["Scale"][0].GetDouble());
			tHabitatRock.m_vScale.y = static_cast<float>(arrChildDesc[1]["Scale"][1].GetDouble());
			tHabitatRock.m_vScale.z = static_cast<float>(arrChildDesc[1]["Scale"][2].GetDouble());

			tHabitatRock.m_vRotation.x = static_cast<float>(arrChildDesc[2]["Rotation"][0].GetDouble());
			tHabitatRock.m_vRotation.y = static_cast<float>(arrChildDesc[2]["Rotation"][1].GetDouble());
			tHabitatRock.m_vRotation.z = static_cast<float>(arrChildDesc[2]["Rotation"][2].GetDouble());

			tHabitatRock.m_vTranslation.x = static_cast<float>(arrChildDesc[3]["Translation"][0].GetDouble());
			tHabitatRock.m_vTranslation.y = static_cast<float>(arrChildDesc[3]["Translation"][1].GetDouble());
			tHabitatRock.m_vTranslation.z = static_cast<float>(arrChildDesc[3]["Translation"][2].GetDouble());

			get<0>(tHabitatRock.m_tpOctreeNodeIndex) = arrChildDesc[4]["Included Octree Node Index"][0].GetUint();
			get<1>(tHabitatRock.m_tpOctreeNodeIndex) = arrChildDesc[4]["Included Octree Node Index"][1].GetUint();
			get<2>(tHabitatRock.m_tpOctreeNodeIndex) = arrChildDesc[4]["Included Octree Node Index"][2].GetUint();

			m_vecHabitatTreeOrRockCaches.push_back(tHabitatRock);
		}
	}

	if (!bBranchEmpty)
	{
		rapidjson::Value& arrWoodListBoxEmpty = doc["Branches Information"];

		FHabitatBranchOrStoneCache tHabitatBranch{};

		tHabitatBranch.m_bActive		= arrWoodListBoxEmpty["Active"].GetBool();
		tHabitatBranch.m_bRespawn		= arrWoodListBoxEmpty["Respawn"].GetBool();
		tHabitatBranch.m_bIsTrigger		= arrWoodListBoxEmpty["IsTrigger"].GetBool();
		tHabitatBranch.m_fRadius		= arrWoodListBoxEmpty["Radius"].GetFloat();
		tHabitatBranch.m_fHalfHeight	= arrWoodListBoxEmpty["HalfHeight"].GetFloat();
		tHabitatBranch.m_fRespawnTime	= arrWoodListBoxEmpty["Respawn Time"].GetFloat();
		tHabitatBranch.m_iHabitatType	= arrWoodListBoxEmpty["Habitat Type"].GetUint();
		tHabitatBranch.m_iMobilityType	= arrWoodListBoxEmpty["Mobility Type"].GetUint();
		tHabitatBranch.m_iColliderType	= arrWoodListBoxEmpty["Collider Type"].GetUint();

		uint32 iWoodCount = arrWoodListBoxEmpty["Total Number of Branches"].GetUint();
		rapidjson::Value& arrIndividualInfo = arrWoodListBoxEmpty["Individual Information"];

		for (uint32 iIndex = 0; iIndex < iWoodCount; iIndex++)
		{
			wstring wstrWoodID = to_wstring(iIndex);
			wstring wstrWoodName = L"Branch" + wstrWoodID;

			rapidjson::Value& arrChildDesc = arrIndividualInfo[CStringUtility::ToString(wstrWoodName).c_str()];

			tHabitatBranch.m_wstrModelFilePath = CStringUtility::ToWideString(arrChildDesc[0]["Model File Path"].GetString());

			tHabitatBranch.m_vScale.x = arrChildDesc[1]["Scale"][0].GetFloat();
			tHabitatBranch.m_vScale.y = arrChildDesc[1]["Scale"][1].GetFloat();
			tHabitatBranch.m_vScale.z = arrChildDesc[1]["Scale"][2].GetFloat();

			tHabitatBranch.m_vRotation.x = arrChildDesc[2]["Rotation"][0].GetFloat();
			tHabitatBranch.m_vRotation.y = arrChildDesc[2]["Rotation"][1].GetFloat();
			tHabitatBranch.m_vRotation.z = arrChildDesc[2]["Rotation"][2].GetFloat();

			tHabitatBranch.m_vTranslation.x = arrChildDesc[3]["Translation"][0].GetFloat();
			tHabitatBranch.m_vTranslation.y = arrChildDesc[3]["Translation"][1].GetFloat();
			tHabitatBranch.m_vTranslation.z = arrChildDesc[3]["Translation"][2].GetFloat();

			get<0>(tHabitatBranch.m_tpOctreeNodeIndex) = arrChildDesc[4]["Included Octree Node Index"][0].GetUint();
			get<1>(tHabitatBranch.m_tpOctreeNodeIndex) = arrChildDesc[4]["Included Octree Node Index"][1].GetUint();
			get<2>(tHabitatBranch.m_tpOctreeNodeIndex) = arrChildDesc[4]["Included Octree Node Index"][2].GetUint();

			m_vecHabitatBranchOrStoneCaches.push_back(tHabitatBranch);
		}
	}
	if (!bStoneEmpty)
	{
		rapidjson::Value& arrStoneListBoxEmpty = doc["Stones Information"];

		FHabitatBranchOrStoneCache tHabitatStone{};

		tHabitatStone.m_bActive			= arrStoneListBoxEmpty["Active"].GetBool();
		tHabitatStone.m_bRespawn		= arrStoneListBoxEmpty["Respawn"].GetBool();
		tHabitatStone.m_bIsTrigger		= arrStoneListBoxEmpty["IsTrigger"].GetBool();
		tHabitatStone.m_fRadius			= arrStoneListBoxEmpty["Radius"].GetFloat();
		tHabitatStone.m_fHalfHeight		= arrStoneListBoxEmpty["HalfHeight"].GetFloat();
		tHabitatStone.m_fRespawnTime	= arrStoneListBoxEmpty["Respawn Time"].GetFloat();
		tHabitatStone.m_iHabitatType	= arrStoneListBoxEmpty["Habitat Type"].GetUint();
		tHabitatStone.m_iMobilityType	= arrStoneListBoxEmpty["Mobility Type"].GetUint();
		tHabitatStone.m_iColliderType	= arrStoneListBoxEmpty["Collider Type"].GetUint();

		uint32 iStoneCount = arrStoneListBoxEmpty["Total Number of Stones"].GetUint();
		rapidjson::Value& arrIndividualInfo = arrStoneListBoxEmpty["Individual Information"];

		for (uint32 iIndex = 0; iIndex < iStoneCount; iIndex++)
		{
			wstring wstrStoneID = to_wstring(iIndex);
			wstring wstrStoneName = L"Stone" + wstrStoneID;

			rapidjson::Value& arrChildDesc = arrIndividualInfo[CStringUtility::ToString(wstrStoneName).c_str()];

			tHabitatStone.m_wstrModelFilePath = CStringUtility::ToWideString(arrChildDesc[0]["Model File Path"].GetString());

			tHabitatStone.m_vScale.x = arrChildDesc[1]["Scale"][0].GetFloat();
			tHabitatStone.m_vScale.y = arrChildDesc[1]["Scale"][1].GetFloat();
			tHabitatStone.m_vScale.z = arrChildDesc[1]["Scale"][2].GetFloat();

			tHabitatStone.m_vRotation.x = arrChildDesc[2]["Rotation"][0].GetFloat();
			tHabitatStone.m_vRotation.y = arrChildDesc[2]["Rotation"][1].GetFloat();
			tHabitatStone.m_vRotation.z = arrChildDesc[2]["Rotation"][2].GetFloat();

			tHabitatStone.m_vTranslation.x = arrChildDesc[3]["Translation"][0].GetFloat();
			tHabitatStone.m_vTranslation.y = arrChildDesc[3]["Translation"][1].GetFloat();
			tHabitatStone.m_vTranslation.z = arrChildDesc[3]["Translation"][2].GetFloat();

			get<0>(tHabitatStone.m_tpOctreeNodeIndex) = arrChildDesc[4]["Included Octree Node Index"][0].GetUint();
			get<1>(tHabitatStone.m_tpOctreeNodeIndex) = arrChildDesc[4]["Included Octree Node Index"][1].GetUint();
			get<2>(tHabitatStone.m_tpOctreeNodeIndex) = arrChildDesc[4]["Included Octree Node Index"][2].GetUint();

			m_vecHabitatBranchOrStoneCaches.push_back(tHabitatStone);
		}
	}
	if (!bPalEmpty)
	{
		rapidjson::Value& arrPalListBoxEmpty = doc["Pals Information"];

		FHabitatPalCache tHabitatPal{};

		tHabitatPal.m_bIsTrigger = arrPalListBoxEmpty["IsTrigger"].GetBool();

		uint32 iPalCount = arrPalListBoxEmpty["Total Number of Pals"].GetUint();
		rapidjson::Value& arrIndividualInfo = arrPalListBoxEmpty["Individual Information"];

		for (uint32 iIndex = 0; iIndex < iPalCount; iIndex++)
		{
			wstring wstrPalID = to_wstring(iIndex);
			wstring wstrPalName = L"Pal" + wstrPalID;

			rapidjson::Value& arrChildDesc = arrIndividualInfo[CStringUtility::ToString(wstrPalName).c_str()];

			tHabitatPal.m_wstrModelFilePath = CStringUtility::ToWideString(arrChildDesc[0]["Model File Path"].GetString());

			tHabitatPal.m_vPosition.x = arrChildDesc[1]["Position"][0].GetFloat();
			tHabitatPal.m_vPosition.y = arrChildDesc[1]["Position"][1].GetFloat();
			tHabitatPal.m_vPosition.z = arrChildDesc[1]["Position"][2].GetFloat();

			get<0>(tHabitatPal.m_tpOctreeNodeIndex) = arrChildDesc[2]["Included Octree Node Index"][0].GetUint();
			get<1>(tHabitatPal.m_tpOctreeNodeIndex) = arrChildDesc[2]["Included Octree Node Index"][1].GetUint();
			get<2>(tHabitatPal.m_tpOctreeNodeIndex) = arrChildDesc[2]["Included Octree Node Index"][2].GetUint();

			m_vecHabitatPalCaches.push_back(tHabitatPal);
		}
	}
	if (!bArchitectureEmpty)
	{
		rapidjson::Value& arrArchitectureListBoxEmpty = doc["Architectures Information"];

		FHabitatArchitectureCache tHabitatArchitecture{};

		tHabitatArchitecture.m_bIsTrigger = arrArchitectureListBoxEmpty["IsTrigger"].GetBool();

		uint32 iArchitectureCount = arrArchitectureListBoxEmpty["Total Number of Architectures"].GetUint();
		rapidjson::Value& arrIndividualInfo = arrArchitectureListBoxEmpty["Individual Information"];

		for (uint32 iIndex = 0; iIndex < iArchitectureCount; iIndex++)
		{
			wstring wstrArchitectureID = to_wstring(iIndex);
			wstring wstrArchitectureName = L"Architecture" + wstrArchitectureID;

			rapidjson::Value& arrChildDesc = arrIndividualInfo[CStringUtility::ToString(wstrArchitectureName).c_str()];

			tHabitatArchitecture.m_wstrModelFilePath = CStringUtility::ToWideString(arrChildDesc[0]["Model File Name"].GetString());

			tHabitatArchitecture.m_vScale.x = arrChildDesc[1]["Scale"][0].GetFloat();
			tHabitatArchitecture.m_vScale.y = arrChildDesc[1]["Scale"][1].GetFloat();
			tHabitatArchitecture.m_vScale.z = arrChildDesc[1]["Scale"][2].GetFloat();

			tHabitatArchitecture.m_vRotation.x = arrChildDesc[2]["Rotation"][0].GetFloat();
			tHabitatArchitecture.m_vRotation.y = arrChildDesc[2]["Rotation"][1].GetFloat();
			tHabitatArchitecture.m_vRotation.z = arrChildDesc[2]["Rotation"][2].GetFloat();

			tHabitatArchitecture.m_vTranslation.x = arrChildDesc[3]["Translation"][0].GetFloat();
			tHabitatArchitecture.m_vTranslation.y = arrChildDesc[3]["Translation"][1].GetFloat();
			tHabitatArchitecture.m_vTranslation.z = arrChildDesc[3]["Translation"][2].GetFloat();

			get<0>(tHabitatArchitecture.m_tpOctreeNodeIndex) = arrChildDesc[4]["Included Octree Node Index"][0].GetUint();
			get<1>(tHabitatArchitecture.m_tpOctreeNodeIndex) = arrChildDesc[4]["Included Octree Node Index"][1].GetUint();
			get<2>(tHabitatArchitecture.m_tpOctreeNodeIndex) = arrChildDesc[4]["Included Octree Node Index"][2].GetUint();

			m_vecHabitatArchitectureCaches.push_back(tHabitatArchitecture);
		}
	}
	if (!bSpotLightEmpty)
	{
		rapidjson::Value& arrSpotLightListBoxEmpty = doc["SpotLights Information"];

		FHabitatSpotLightCache tHabitatSpotLight{};

		uint32 iSpotLightCount = arrSpotLightListBoxEmpty["Total Number of SpotLights"].GetUint();
		rapidjson::Value& arrIndividualInfo = arrSpotLightListBoxEmpty["Individual Information"];

		for (uint32 iIndex = 0; iIndex < iSpotLightCount; iIndex++)
		{
			wstring wstrSpotLightID = to_wstring(iIndex);
			wstring wstrSpotLightName = L"SpotLight" + wstrSpotLightID;

			rapidjson::Value& arrChildDesc = arrIndividualInfo[CStringUtility::ToString(wstrSpotLightName).c_str()];

			tHabitatSpotLight.m_fRadius = arrChildDesc[0]["Radius"].GetFloat();
			tHabitatSpotLight.m_fCosAngle = arrChildDesc[1]["CosAngle"].GetFloat();

			tHabitatSpotLight.m_vEnergy.x = arrChildDesc[2]["Energy"][0].GetFloat();
			tHabitatSpotLight.m_vEnergy.y = arrChildDesc[2]["Energy"][1].GetFloat();
			tHabitatSpotLight.m_vEnergy.z = arrChildDesc[2]["Energy"][2].GetFloat();

			tHabitatSpotLight.m_vPosition.x = arrChildDesc[3]["Position"][0].GetFloat();
			tHabitatSpotLight.m_vPosition.y = arrChildDesc[3]["Position"][1].GetFloat();
			tHabitatSpotLight.m_vPosition.z = arrChildDesc[3]["Position"][2].GetFloat();

			tHabitatSpotLight.m_vDirection.x = arrChildDesc[4]["Direction"][0].GetFloat();
			tHabitatSpotLight.m_vDirection.y = arrChildDesc[4]["Direction"][1].GetFloat();
			tHabitatSpotLight.m_vDirection.z = arrChildDesc[4]["Direction"][2].GetFloat();

			get<0>(tHabitatSpotLight.m_tpOctreeNodeIndex) = arrChildDesc[5]["Included Octree Node Index"][0].GetUint();
			get<1>(tHabitatSpotLight.m_tpOctreeNodeIndex) = arrChildDesc[5]["Included Octree Node Index"][1].GetUint();
			get<2>(tHabitatSpotLight.m_tpOctreeNodeIndex) = arrChildDesc[5]["Included Octree Node Index"][2].GetUint();

			m_vecHabitatSpotLightCaches.push_back(tHabitatSpotLight);
		}
	}
	if (!bPointLightEmpty)
	{
		rapidjson::Value& arrPointLightListBoxEmpty = doc["PointLights Information"];

		FHabitatPointLightCache tHabitatPointLight{};

		uint32 iPointLightCount = arrPointLightListBoxEmpty["Total Number of PointLights"].GetUint();
		rapidjson::Value& arrIndividualInfo = arrPointLightListBoxEmpty["Individual Information"];

		for (uint32 iIndex = 0; iIndex < iPointLightCount; iIndex++)
		{
			wstring wstrPointLightID = to_wstring(iIndex);
			wstring wstrPointLightName = L"PointLight" + wstrPointLightID;

			rapidjson::Value& arrChildDesc = arrIndividualInfo[CStringUtility::ToString(wstrPointLightID).c_str()];

			tHabitatPointLight.m_fRadius = arrChildDesc[0]["Radius"].GetFloat();

			tHabitatPointLight.m_vEnergy.x = arrChildDesc[1]["Energy"][0].GetFloat();
			tHabitatPointLight.m_vEnergy.y = arrChildDesc[1]["Energy"][1].GetFloat();
			tHabitatPointLight.m_vEnergy.z = arrChildDesc[1]["Energy"][2].GetFloat();

			tHabitatPointLight.m_vPosition.x = arrChildDesc[2]["Position"][0].GetFloat();
			tHabitatPointLight.m_vPosition.y = arrChildDesc[2]["Position"][1].GetFloat();
			tHabitatPointLight.m_vPosition.z = arrChildDesc[2]["Position"][2].GetFloat();

			get<0>(tHabitatPointLight.m_tpOctreeNodeIndex) = arrChildDesc[3]["Included Octree Node Index"][0].GetUint();
			get<1>(tHabitatPointLight.m_tpOctreeNodeIndex) = arrChildDesc[3]["Included Octree Node Index"][1].GetUint();
			get<2>(tHabitatPointLight.m_tpOctreeNodeIndex) = arrChildDesc[3]["Included Octree Node Index"][2].GetUint();

			m_vecHabitatPointLightCaches.push_back(tHabitatPointLight);
		}
	}

	if (!bDirectionalLightEmpty)
	{
		rapidjson::Value& arrDirectionalLightListBoxEmpty = doc["DirectionalLights Information"];

		FHabitatDirectionalLightCache tHabitatDirectionalLight{};

		uint32 iDirectionalLightCount = arrDirectionalLightListBoxEmpty["Total Number of DirectionalLights"].GetUint();
		rapidjson::Value& arrIndividualInfo = arrDirectionalLightListBoxEmpty["Individual Information"];

		for (uint32 iIndex = 0; iIndex < iDirectionalLightCount; iIndex++)
		{
			wstring wstrDirectionalLightID = to_wstring(iIndex);
			wstring wstrDirectionalLightName = L"DirectionalLight" + wstrDirectionalLightID;

			rapidjson::Value& arrChildDesc = arrIndividualInfo[CStringUtility::ToString(wstrDirectionalLightID).c_str()];

			tHabitatDirectionalLight.m_fSolidAngle = arrChildDesc[0]["SolidAngle"].GetFloat();
			tHabitatDirectionalLight.m_fPerceivedRadius = arrChildDesc[0]["PerceivedRadius"].GetFloat();
			tHabitatDirectionalLight.m_fPerceivedDistance = arrChildDesc[0]["PerceivedDistance"].GetFloat();

			tHabitatDirectionalLight.m_vEnergy.x = arrChildDesc[1]["Energy"][0].GetFloat();
			tHabitatDirectionalLight.m_vEnergy.y = arrChildDesc[1]["Energy"][1].GetFloat();
			tHabitatDirectionalLight.m_vEnergy.z = arrChildDesc[1]["Energy"][2].GetFloat();

			tHabitatDirectionalLight.m_vDirection.x = arrChildDesc[2]["Direction"][0].GetFloat();
			tHabitatDirectionalLight.m_vDirection.y = arrChildDesc[2]["Direction"][1].GetFloat();
			tHabitatDirectionalLight.m_vDirection.z = arrChildDesc[2]["Direction"][2].GetFloat();

			get<0>(tHabitatDirectionalLight.m_tpOctreeNodeIndex) = arrChildDesc[3]["Included Octree Node Index"][0].GetUint();
			get<1>(tHabitatDirectionalLight.m_tpOctreeNodeIndex) = arrChildDesc[3]["Included Octree Node Index"][1].GetUint();
			get<2>(tHabitatDirectionalLight.m_tpOctreeNodeIndex) = arrChildDesc[3]["Included Octree Node Index"][2].GetUint();

			m_vecHabitatDirectionalLightCaches.push_back(tHabitatDirectionalLight);
		}
	}

	if (!bCameraEmpty)
	{
		rapidjson::Value& arrCameraListBoxEmpty = doc["Cameras Information"];

		FHabitatCameraFrustumCache tHabitatCameraFrustum{};

		uint32 iCameraCount = arrCameraListBoxEmpty["Total Number of Cameras"].GetUint();
		rapidjson::Value& arrIndividualInfo = arrCameraListBoxEmpty["Individual Information"];

		for (uint32 iIndex = 0; iIndex < iCameraCount; iIndex++)
		{
			wstring wstrCameraID = to_wstring(iIndex);
			wstring wstrCameraName = L"Camera" + wstrCameraID;

			rapidjson::Value& arrChildDesc = arrIndividualInfo[CStringUtility::ToString(wstrCameraName).c_str()];

			tHabitatCameraFrustum.m_fRightSlope = arrChildDesc[0]["RightSlope"].GetFloat();
			tHabitatCameraFrustum.m_fLeftSlope = arrChildDesc[1]["LeftSlope"].GetFloat();
			tHabitatCameraFrustum.m_fTopSlope = arrChildDesc[2]["TopSlope"].GetFloat();
			tHabitatCameraFrustum.m_fBottomSlope = arrChildDesc[3]["BottomSlope"].GetFloat();
			tHabitatCameraFrustum.m_fNearPlane = arrChildDesc[4]["NearPlane"].GetFloat();
			tHabitatCameraFrustum.m_fFarPlane = arrChildDesc[5]["FarPlane"].GetFloat();

			tHabitatCameraFrustum.m_vCenter.x = arrChildDesc[6]["Center"][0].GetFloat();
			tHabitatCameraFrustum.m_vCenter.y = arrChildDesc[6]["Center"][1].GetFloat();
			tHabitatCameraFrustum.m_vCenter.z = arrChildDesc[6]["Center"][2].GetFloat();

			tHabitatCameraFrustum.m_vRadian.x = arrChildDesc[7]["Radian"][0].GetFloat();
			tHabitatCameraFrustum.m_vRadian.y = arrChildDesc[7]["Radian"][1].GetFloat();
			tHabitatCameraFrustum.m_vRadian.z = arrChildDesc[7]["Radian"][2].GetFloat();

			m_vecHabitatCameraFrustumCaches.push_back(tHabitatCameraFrustum);
		}
	}
	*/
	return S_OK;
}
