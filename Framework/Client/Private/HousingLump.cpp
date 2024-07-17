// Default
#include "pch.h"
#include "HousingLump.h"
#include "StringUtility.h"

// Module, Manager
#include "ClientModule.h"
#include "BuildManager.h"

// GameObject
#include "Housing.h"

std::shared_ptr<CHousingLump> Client::CHousingLump::Create(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	shared_ptr<CHousingLump> spInstance = make_shared<CHousingLump>();
	CHECKF(spInstance, L"CHousingLump : Create Failed");
	spInstance->Initialize(_spGameObjectDesc);

	return spInstance;
}

HRESULT Client::CHousingLump::Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	if (FAILED(CGameObject::Initialize(_spGameObjectDesc))) { return E_FAIL; }

	shared_ptr<FHousingLumpDesc> spHousingLumpDesc = static_pointer_cast<FHousingLumpDesc>(_spGameObjectDesc);

	// 하우징 덩어리를 빌드 매니저에 등록합니다.
	m_spBuildManager = CClientModule::GetInstance()->GetBuildManager();
	m_spBuildManager->AddHousingLump(GetGameObjectID(), static_pointer_cast<CHousingLump>(shared_from_this()));

	return S_OK;
}

HRESULT Client::CHousingLump::BeginPlay()
{
	if (FAILED(CGameObject::BeginPlay())) { return E_FAIL; }
	return S_OK;
}

int32 Client::CHousingLump::PreTick(float _fDeltaSeconds)
{
	switch (CGameObject::PreTick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}
	return 0;
}

int32 Client::CHousingLump::Tick(float _fDeltaSeconds)
{
	switch (CGameObject::Tick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	// 토대가 1개도 없는 경우, 이 덩어리는 파괴됩니다.
	if (1 > m_iNumFoundation) { return -1; }

	return 0;
}

int32 Client::CHousingLump::PostTick(float _fDeltaSeconds)
{
	switch (CGameObject::PostTick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}
	return 0;
}

HRESULT Client::CHousingLump::EndPlay()
{
	if (FAILED(CGameObject::EndPlay())) { return E_FAIL; }
	return S_OK;
}

void Client::CHousingLump::Release()
{
	// 이 하우징 덩어리를 BuildManager에서 삭제합니다.
	// ENSUREF(m_spBuildManager->RemoveHousingLump(GetGameObjectID()), L"Failed to RemoveHousingLump : CHousingLump->Release()");

	// ※연결된 하우징을 모두 파괴하는 구문을 추가해야합니다.※
	//for (auto& pair : m_umapBuildIndices)
	//{
	//	if (pair.second)
	//	{
	//		pair.second->Release();
	//		pair.second = nullptr;
	//	}
	//}
	
	CGameObject::Release();
}

HRESULT Client::CHousingLump::AddHousing(tuple<int32, int32, int32> _tpIndex, std::shared_ptr<CHousing> _spHousing)
{
	if (m_umapBuildIndices.contains(_tpIndex)) { return E_FAIL; }

	if (EBuildType::FOUNDATION == _spHousing->GetBuildType())
	{
		++m_iNumFoundation;
	}
	else if (EBuildType::WALL == _spHousing->GetBuildType() ||
			 EBuildType::DOOR == _spHousing->GetBuildType() ||
			 EBuildType::STAIRS == _spHousing->GetBuildType())
	{
		// 문과 벽과 계단은 인덱스에 추가하지 않습니다.
		return S_OK;
	}

	m_umapBuildIndices.emplace(_tpIndex, _spHousing);

	return S_OK;
}

HRESULT Client::CHousingLump::RemoveHousing(tuple<int32, int32, int32> _tpIndex)
{
	if (!m_umapBuildIndices.contains(_tpIndex)) { return E_FAIL; }

	if (EBuildType::FOUNDATION == m_umapBuildIndices[_tpIndex]->GetBuildType())
	{
		--m_iNumFoundation;
	}

	m_umapBuildIndices.erase(_tpIndex);

	// 만약 토대의 개수가 0이라면 하우징 덩어리를 제거합니다.
	if (0 >= m_iNumFoundation)
	{
		SetActive(false);
		m_spBuildManager->RemoveHousingLump(GetGameObjectID());
	}

	return S_OK;
}

shared_ptr<CHousing> Client::CHousingLump::FindHousing(tuple<int32, int32, int32> _tpIndex)
{
	if (!m_umapBuildIndices.contains(_tpIndex)) { return nullptr; }
	
	return m_umapBuildIndices[_tpIndex];
}

HRESULT Client::CHousingLump::SaveHousingLump()
{
	rapidjson::StringBuffer buffer;
	rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);

	writer.StartObject();

	writer.Key("Housings");
	writer.StartArray();

	shared_ptr<CHousing> spHousing = m_umapBuildIndices[tuple(0, 0, 0)];
	CHECKF(spHousing, L"Housing is nullptr");

	spHousing->SaveHousing(writer);

	writer.EndArray();

	writer.EndObject();

	wstring wstrFileName = L"Bowling Alley.json";

	ofstream osJsonpath(L"../../Resource/" + wstrFileName);
	if (!osJsonpath.is_open())
	{
		throw runtime_error("failed to save json file : HousingLump");
	}

	osJsonpath << buffer.GetString();
	osJsonpath.close();

	return S_OK;
}

bool Client::CHousingLump::IsSatisfiedSupportingTheory(tuple<int32, int32, int32> _tpIndex, EBuildType _eBuildType)
{
	/* _tpIndex = 하우징이 건축될 위치 인덱스 */
	/* 이 인덱스를 기준으로 지지대 원리를 만족하는 위치에 1층에 토대가 있는지 검사합니다. */

	switch (_eBuildType)
	{
	/* 지붕, 계단 */
	case EBuildType::ROOF:
	case EBuildType::STAIRS:
	{
		#pragma region 지붕, 계단 지지대 원리 검사
		// [가운데] 검사
		if (m_umapBuildIndices.contains(tuple(0, get<1>(_tpIndex), get<2>(_tpIndex))))
		{
			if (EBuildType::FOUNDATION == m_umapBuildIndices[tuple(0, get<1>(_tpIndex), get<2>(_tpIndex))]->GetBuildType())
				return true;
		}

		// [좌 -1, -2] 검사
		for (int32 i = 1; i < 3; ++i)
		{
			if (m_umapBuildIndices.contains(tuple(0, get<1>(_tpIndex) - i, get<2>(_tpIndex))))
			{
				if (EBuildType::FOUNDATION == m_umapBuildIndices[tuple(0, get<1>(_tpIndex) - i, get<2>(_tpIndex))]->GetBuildType())
					return true;
			}
		}

		// [우 +1, +2] 검사
		for (int32 i = 1; i < 3; ++i)
		{
			if (m_umapBuildIndices.contains(tuple(0, get<1>(_tpIndex) + i, get<2>(_tpIndex))))
			{
				if (EBuildType::FOUNDATION == m_umapBuildIndices[tuple(0, get<1>(_tpIndex) + i, get<2>(_tpIndex))]->GetBuildType())
					return true;
			}
		}

		// [하 -1, -2] 검사
		for (int32 i = 1; i < 3; ++i)
		{
			if (m_umapBuildIndices.contains(tuple(0, get<1>(_tpIndex), get<2>(_tpIndex) - i)))
			{
				if (EBuildType::FOUNDATION == m_umapBuildIndices[tuple(0, get<1>(_tpIndex), get<2>(_tpIndex) - i)]->GetBuildType())
					return true;
			}
		}

		// [상 +1, +2] 검사
		for (int32 i = 1; i < 3; ++i)
		{
			if (m_umapBuildIndices.contains(tuple(0, get<1>(_tpIndex), get<2>(_tpIndex) + i)))
			{
				if (EBuildType::FOUNDATION == m_umapBuildIndices[tuple(0, get<1>(_tpIndex), get<2>(_tpIndex) + i)]->GetBuildType())
					return true;
			}
		}

		// 좌 대각선 위 검사
		if (m_umapBuildIndices.contains(tuple(0, get<1>(_tpIndex) - 1, get<2>(_tpIndex) + 1)))
		{
			if (EBuildType::FOUNDATION == m_umapBuildIndices[tuple(0, get<1>(_tpIndex) - 1, get<2>(_tpIndex) + 1)]->GetBuildType())
				return true;
		}

		// 좌 대각선 아래 검사
		if (m_umapBuildIndices.contains(tuple(0, get<1>(_tpIndex) - 1, get<2>(_tpIndex) - 1)))
		{
			if (EBuildType::FOUNDATION == m_umapBuildIndices[tuple(0, get<1>(_tpIndex) - 1, get<2>(_tpIndex) - 1)]->GetBuildType())
				return true;
		}

		// 우 대각선 위 검사
		if (m_umapBuildIndices.contains(tuple(0, get<1>(_tpIndex) + 1, get<2>(_tpIndex) + 1)))
		{
			if (EBuildType::FOUNDATION == m_umapBuildIndices[tuple(0, get<1>(_tpIndex) + 1, get<2>(_tpIndex) + 1)]->GetBuildType())
				return true;
		}

		// 우 대각선 아래 검사
		if (m_umapBuildIndices.contains(tuple(0, get<1>(_tpIndex) + 1, get<2>(_tpIndex) - 1)))
		{
			if (EBuildType::FOUNDATION == m_umapBuildIndices[tuple(0, get<1>(_tpIndex) + 1, get<2>(_tpIndex) - 1)]->GetBuildType())
				return true;
		}
		#pragma endregion
		break;
	}
	case EBuildType::WALL:
	case EBuildType::DOOR:
	{
		#pragma region 벽, 문 지지대 원리 검사
		// [가운데]
		if (m_umapBuildIndices.contains(tuple(0, get<1>(_tpIndex), get<2>(_tpIndex))))
		{
			if (EBuildType::FOUNDATION == m_umapBuildIndices[tuple(0, get<1>(_tpIndex), get<2>(_tpIndex))]->GetBuildType())
				return true;
		}

		// [좌 -1]
		if (m_umapBuildIndices.contains(tuple(0, get<1>(_tpIndex) - 1, get<2>(_tpIndex))))
		{
			if (EBuildType::FOUNDATION == m_umapBuildIndices[tuple(0, get<1>(_tpIndex) - 1, get<2>(_tpIndex))]->GetBuildType())
				return true;
		}

		// [우 +1]
		if (m_umapBuildIndices.contains(tuple(0, get<1>(_tpIndex) + 1, get<2>(_tpIndex))))
		{
			if (EBuildType::FOUNDATION == m_umapBuildIndices[tuple(0, get<1>(_tpIndex) + 1, get<2>(_tpIndex))]->GetBuildType())
				return true;
		}

		// [하 -1]
		if (m_umapBuildIndices.contains(tuple(0, get<1>(_tpIndex), get<2>(_tpIndex) - 1)))
		{
			if (EBuildType::FOUNDATION == m_umapBuildIndices[tuple(0, get<1>(_tpIndex), get<2>(_tpIndex) - 1)]->GetBuildType())
				return true;
		}

		// [상 +1]
		if (m_umapBuildIndices.contains(tuple(0, get<1>(_tpIndex), get<2>(_tpIndex) + 1)))
		{
			if (EBuildType::FOUNDATION == m_umapBuildIndices[tuple(0, get<1>(_tpIndex), get<2>(_tpIndex) + 1)]->GetBuildType())
				return true;
		}

		// 좌 대각선 위 검사
		if (m_umapBuildIndices.contains(tuple(0, get<1>(_tpIndex) - 1, get<2>(_tpIndex) + 1)))
		{
			if (EBuildType::FOUNDATION == m_umapBuildIndices[tuple(0, get<1>(_tpIndex) - 1, get<2>(_tpIndex) + 1)]->GetBuildType())
				return true;
		}

		// 좌 대각선 아래 검사
		if (m_umapBuildIndices.contains(tuple(0, get<1>(_tpIndex) - 1, get<2>(_tpIndex) - 1)))
		{
			if (EBuildType::FOUNDATION == m_umapBuildIndices[tuple(0, get<1>(_tpIndex) - 1, get<2>(_tpIndex) - 1)]->GetBuildType())
				return true;
		}

		// 우 대각선 위 검사
		if (m_umapBuildIndices.contains(tuple(0, get<1>(_tpIndex) + 1, get<2>(_tpIndex) + 1)))
		{
			if (EBuildType::FOUNDATION == m_umapBuildIndices[tuple(0, get<1>(_tpIndex) + 1, get<2>(_tpIndex) + 1)]->GetBuildType())
				return true;
		}

		// 우 대각선 아래 검사
		if (m_umapBuildIndices.contains(tuple(0, get<1>(_tpIndex) + 1, get<2>(_tpIndex) - 1)))
		{
			if (EBuildType::FOUNDATION == m_umapBuildIndices[tuple(0, get<1>(_tpIndex) + 1, get<2>(_tpIndex) - 1)]->GetBuildType())
				return true;
		}
		#pragma endregion
		break;
	}
	default:
		return true;
	}

	return false;
}