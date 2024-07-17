// Default
#include "pch.h"
#include "HousingPrefab.h"

// Module, Manager
#include "EngineModule.h"
#include "BuildManager.h"
#include "CameraManager.h"
#include "InputManager.h"

// GameObject
#include "Player.h"
#include "Housing.h"
#include "HousingLump.h"

// Component
#include "MeshComponent.h"
#include "ColliderComponent.h"

std::shared_ptr<CHousingPrefab> Client::CHousingPrefab::Create(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	shared_ptr<CHousingPrefab> spInstance = make_shared<CHousingPrefab>();
	CHECKF(spInstance, L"CHousingPrefab : Create Failed");
	spInstance->Initialize(_spGameObjectDesc);

	return spInstance;
}

HRESULT Client::CHousingPrefab::Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	if (FAILED(CArchitecturePrefab::Initialize(_spGameObjectDesc))) { return E_FAIL; }

	/* gameobject */
	shared_ptr<FHousingPrefabDesc> spHousingPrefabDesc = static_pointer_cast<FHousingPrefabDesc>(_spGameObjectDesc);

	m_eBuildType			= spHousingPrefabDesc->m_eBuildType;
	m_umapNeededBuildTypes	= spHousingPrefabDesc->m_umapNeededBuildTypes;

	m_bHousingPrefab = true;

	return S_OK;
}

HRESULT Client::CHousingPrefab::BeginPlay()
{
	if (FAILED(CArchitecturePrefab::BeginPlay())) { return E_FAIL; }

	return S_OK;
}

int32 Client::CHousingPrefab::PreTick(float _fDeltaSeconds)
{
	switch (CArchitecturePrefab::PreTick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	// 회전
	if (CEngineModule::GetInstance()->GetInputManager()->IsKeyDown(DIK_Q))
	{
		SetRotateAccumulated(Vector3(0.f, 1.f, 0.f), -90.f);
	}
	if (CEngineModule::GetInstance()->GetInputManager()->IsKeyDown(DIK_E))
	{
		SetRotateAccumulated(Vector3(0.f, 1.f, 0.f), 90.f);
	}

	return 0;
}

int32 Client::CHousingPrefab::Tick(float _fDeltaSeconds)
{
	switch (CArchitecturePrefab::Tick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	AttachToNearHousing();

	return 0;
}

int32 Client::CHousingPrefab::PostTick(float _fDeltaSeconds)
{
	switch (CArchitecturePrefab::PostTick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}
	return 0;
}

HRESULT Client::CHousingPrefab::EndPlay()
{
	if (FAILED(CArchitecturePrefab::EndPlay())) { return E_FAIL; }
	return S_OK;
}

void Client::CHousingPrefab::Release()
{
	CArchitecturePrefab::Release();
}

void Client::CHousingPrefab::AttachToNearHousing()
{
	// 빌드 매니저에서 현재 필드에 있는 하우징 중 가장 가까운 하우징을 검색해옵니다.
	m_wpAttachedHousing = m_spBuildManager->FindNearHousing(GetPosition().value(), 6.f);

	if (m_wpAttachedHousing.expired() || !m_umapNeededBuildTypes.contains(m_wpAttachedHousing.lock()->GetBuildType()))
	{ 
		// 필요 빌드 타입이 맞지 않는 경우, 부착되지 않아도 됩니다.
		SetNoneAttached();
		return;
	}

	Vector3 vHousingPos = m_wpAttachedHousing.lock()->GetPosition().value();
	Vector3 vBuildSpot = m_wpPlayer.lock()->GetPosition().value() + CEngineModule::GetInstance()->GetCameraManager()->GetCurCameraWorldForwardVector() * 6.f;

	Vector3 vHousingLook		= XMVector3Normalize(m_wpAttachedHousing.lock()->GetForwardVector().value());
	float	fHousingLookScale	= m_wpAttachedHousing.lock()->GetForwardVector().value().Length();
	Vector3 vHousingRight		= XMVector3Normalize(m_wpAttachedHousing.lock()->GetRightVector().value());
	float	fHousingRightScale	= m_wpAttachedHousing.lock()->GetRightVector().value().Length();
	Vector3 vHousingUp			= XMVector3Normalize(m_wpAttachedHousing.lock()->GetUpVector().value());
	float	fHousingUpScale		= m_wpAttachedHousing.lock()->GetUpVector().value().Length();

	Vector3 vHousingToBuildSpot	= XMVector3Normalize(vBuildSpot - vHousingPos);

	Vector3 vCross = vHousingLook.Cross(vHousingToBuildSpot);
	float	fAngle = XMConvertToDegrees(acos(vHousingLook.Dot(vHousingToBuildSpot)));

	if (vCross.y < 0.f) { fAngle = 360.f - fAngle; }

	// 프리팹의 현재 위치에 따라 붙여질 방향을 결정합니다.
	m_eBuildDir = DecideBuildDir(m_eBuildType, m_wpAttachedHousing.lock()->GetBuildType(), fAngle);

	tuple<int32, int32, int32> tpIndex = m_wpAttachedHousing.lock()->GetBuildIndex();
	auto& [iFloor, iX, iZ] = tpIndex;

	switch (m_eBuildDir)
	{
	#pragma region LOOK
	case EBuildDir::LOOK:
	case EBuildDir::LOOK_UP:
	case EBuildDir::LOOK_DOWN:
	{
		switch (m_eBuildDir)
		{
		case EBuildDir::LOOK:
		{
			if (EBuildType::WALL == m_wpAttachedHousing.lock()->GetBuildType() || EBuildType::DOOR == m_wpAttachedHousing.lock()->GetBuildType())
			{
				tuple<int32, int32, int32> tpIndexFloorZero = tuple(0, get<1>(tpIndex), get<2>(tpIndex));
				shared_ptr<CHousing> spIndexedHousing = m_wpAttachedHousing.lock()->GetHousingLump()->FindHousing(tpIndexFloorZero);
				if (!spIndexedHousing)
				{
					SetNoneAttached();
					return;
				}
				
				Vector3 vIndexPos = spIndexedHousing->GetPosition().value();

				m_iFloor = m_wpAttachedHousing.lock()->GetFloor() + 1;
				SetPosition(vIndexPos + vHousingUp * 3.f * fHousingUpScale * (float)(m_iFloor - 1));
			}
			else if (EBuildType::STAIRS == m_wpAttachedHousing.lock()->GetBuildType())
			{
				m_iFloor = m_wpAttachedHousing.lock()->GetFloor() + 1;
				SetPosition(vHousingPos + vHousingUp * 3.f * fHousingUpScale + vHousingLook * 4.f * fHousingLookScale);

				if (m_eBuildType != EBuildType::WALL && m_eBuildType != EBuildType::DOOR)
					++iZ;
			}
			else
			{
				SetPosition(vHousingPos + vHousingLook * 4.f * fHousingLookScale);

				if (m_eBuildType != EBuildType::WALL && m_eBuildType != EBuildType::DOOR)
					++iZ;
			}

			break;
		}
		case EBuildDir::LOOK_UP:
		{
			if (EBuildType::WALL == m_wpAttachedHousing.lock()->GetBuildType() || EBuildType::DOOR == m_wpAttachedHousing.lock()->GetBuildType())
			{
				m_iFloor = m_wpAttachedHousing.lock()->GetFloor() + 1;
				SetPosition(vHousingPos + vHousingUp * 3.f * fHousingUpScale);
			}
			else
			{
				m_iFloor = m_wpAttachedHousing.lock()->GetFloor() + 1;
				SetPosition(vHousingPos + vHousingLook * 2.f * fHousingLookScale);
			}

			break;
		}
		case EBuildDir::LOOK_DOWN:
		{
			if (EBuildType::WALL == m_wpAttachedHousing.lock()->GetBuildType() || EBuildType::DOOR == m_wpAttachedHousing.lock()->GetBuildType())
			{
				m_iFloor = m_wpAttachedHousing.lock()->GetFloor() - 1;
				SetPosition(vHousingPos - vHousingUp * 3.f * fHousingUpScale);
			}
			else
			{
				m_iFloor = m_wpAttachedHousing.lock()->GetFloor() - 1;
				SetPosition(vHousingPos + vHousingLook * 2.f * fHousingLookScale - 3.f * vHousingUp * fHousingUpScale);
			}

			break;
		}
		}

		break;
	}
	#pragma endregion
	#pragma region RIGHT
	case EBuildDir::RIGHT: 
	case EBuildDir::RIGHT_UP:
	case EBuildDir::RIGHT_DOWN:
	{
		switch (m_eBuildDir)
		{
		case EBuildDir::RIGHT:
		{
			if (EBuildType::WALL == m_wpAttachedHousing.lock()->GetBuildType() || EBuildType::DOOR == m_wpAttachedHousing.lock()->GetBuildType())
			{
				tuple<int32, int32, int32> tpIndexFloorZero = tuple(0, get<1>(tpIndex), get<2>(tpIndex));
				shared_ptr<CHousing> spIndexedHousing = m_wpAttachedHousing.lock()->GetHousingLump()->FindHousing(tpIndexFloorZero);
				if (!spIndexedHousing)
				{
					SetNoneAttached();
					return;
				}

				Vector3 vIndexPos = spIndexedHousing->GetPosition().value();

				m_iFloor = m_wpAttachedHousing.lock()->GetFloor() + 1;
				SetPosition(vIndexPos + vHousingUp * 3.f * fHousingUpScale * (float)(m_iFloor - 1));
			}
			else if (EBuildType::STAIRS == m_wpAttachedHousing.lock()->GetBuildType())
			{
				m_iFloor = m_wpAttachedHousing.lock()->GetFloor() + 1;
				SetPosition(vHousingPos + vHousingUp * 3.f * fHousingUpScale + vHousingRight * 4.f * fHousingRightScale);

				if (m_eBuildType != EBuildType::WALL && m_eBuildType != EBuildType::DOOR)
					++iX;
			}
			else
			{
				SetPosition(vHousingPos + vHousingRight * 4.f * fHousingRightScale);

				if (m_eBuildType != EBuildType::WALL && m_eBuildType != EBuildType::DOOR)
					++iX;
			}

			break;
		}
		case EBuildDir::RIGHT_UP:
		{
			if (EBuildType::WALL == m_wpAttachedHousing.lock()->GetBuildType() || EBuildType::DOOR == m_wpAttachedHousing.lock()->GetBuildType())
			{
				m_iFloor = m_wpAttachedHousing.lock()->GetFloor() + 1;
				SetPosition(vHousingPos + vHousingUp * 3.f * fHousingUpScale);
			}
			else
			{
				m_iFloor = m_wpAttachedHousing.lock()->GetFloor() + 1;
				SetPosition(vHousingPos + vHousingRight * 2.f * fHousingRightScale);
			}

			break;
		}
		case EBuildDir::RIGHT_DOWN:
		{
			if (EBuildType::WALL == m_wpAttachedHousing.lock()->GetBuildType() || EBuildType::DOOR == m_wpAttachedHousing.lock()->GetBuildType())
			{
				m_iFloor = m_wpAttachedHousing.lock()->GetFloor() - 1;
				SetPosition(vHousingPos - vHousingUp * 3.f * fHousingUpScale);
			}
			else
			{
				m_iFloor = m_wpAttachedHousing.lock()->GetFloor() - 1;
				SetPosition(vHousingPos + vHousingRight * 2.f * fHousingRightScale - 3.f * vHousingUp * fHousingUpScale);
			}

			break;
		}
		}
		break;
	}
	#pragma endregion
	#pragma region BACK
	case EBuildDir::BACK:
	case EBuildDir::BACK_UP:
	case EBuildDir::BACK_DOWN:
	{
		switch (m_eBuildDir)
		{
		case EBuildDir::BACK:
		{
			if (EBuildType::WALL == m_wpAttachedHousing.lock()->GetBuildType() || EBuildType::DOOR == m_wpAttachedHousing.lock()->GetBuildType())
			{
				tuple<int32, int32, int32> tpIndexFloorZero = tuple(0, get<1>(tpIndex), get<2>(tpIndex));
				shared_ptr<CHousing> spIndexedHousing = m_wpAttachedHousing.lock()->GetHousingLump()->FindHousing(tpIndexFloorZero);
				if (!spIndexedHousing)
				{
					SetNoneAttached();
					return;
				}

				Vector3 vIndexPos = spIndexedHousing->GetPosition().value();

				m_iFloor = m_wpAttachedHousing.lock()->GetFloor() + 1;
				SetPosition(vIndexPos + vHousingUp * 3.f * fHousingUpScale * (float)(m_iFloor - 1));
			}
			else if (EBuildType::STAIRS == m_wpAttachedHousing.lock()->GetBuildType())
			{
				m_iFloor = m_wpAttachedHousing.lock()->GetFloor() + 1;
				SetPosition(vHousingPos + vHousingUp * 3.f * fHousingUpScale - vHousingLook * 4.f * fHousingLookScale);

				if (m_eBuildType != EBuildType::WALL && m_eBuildType != EBuildType::DOOR)
					--iZ;
			}
			else
			{
				SetPosition(vHousingPos - vHousingLook * 4.f * fHousingLookScale);

				if (m_eBuildType != EBuildType::WALL && m_eBuildType != EBuildType::DOOR)
					--iZ;
			}

			break;
		}
		case EBuildDir::BACK_UP:
		{
			if (EBuildType::WALL == m_wpAttachedHousing.lock()->GetBuildType() || EBuildType::DOOR == m_wpAttachedHousing.lock()->GetBuildType())
			{
				m_iFloor = m_wpAttachedHousing.lock()->GetFloor() + 1;
				SetPosition(vHousingPos + vHousingUp * 3.f * fHousingUpScale);
			}
			else
			{
				m_iFloor = m_wpAttachedHousing.lock()->GetFloor() + 1;
				SetPosition(vHousingPos - vHousingLook * 2.f * fHousingLookScale);
			}

			break;
		}
		case EBuildDir::BACK_DOWN:
		{
			if (EBuildType::WALL == m_wpAttachedHousing.lock()->GetBuildType() || EBuildType::DOOR == m_wpAttachedHousing.lock()->GetBuildType())
			{
				m_iFloor = m_wpAttachedHousing.lock()->GetFloor() - 1;
				SetPosition(vHousingPos - vHousingUp * 3.f * fHousingUpScale);
			}
			else
			{
				m_iFloor = m_wpAttachedHousing.lock()->GetFloor() - 1;
				SetPosition(vHousingPos - vHousingLook * 2.f * fHousingLookScale - 3.f * vHousingUp * fHousingUpScale);
			}

			break;
		}
		}
		break;
	}
	#pragma endregion
	#pragma region LEFT
	case EBuildDir::LEFT:
	case EBuildDir::LEFT_UP:
	case EBuildDir::LEFT_DOWN:
	{
		switch (m_eBuildDir)
		{
		case EBuildDir::LEFT:
		{
			if (EBuildType::WALL == m_wpAttachedHousing.lock()->GetBuildType() || EBuildType::DOOR == m_wpAttachedHousing.lock()->GetBuildType())
			{
				tuple<int32, int32, int32> tpIndexFloorZero = tuple(0, get<1>(tpIndex), get<2>(tpIndex));
				shared_ptr<CHousing> spIndexedHousing = m_wpAttachedHousing.lock()->GetHousingLump()->FindHousing(tpIndexFloorZero);
				if (!spIndexedHousing)
				{
					SetNoneAttached();
					return;
				}

				Vector3 vIndexPos = spIndexedHousing->GetPosition().value();

				m_iFloor = m_wpAttachedHousing.lock()->GetFloor() + 1;
				SetPosition(vIndexPos + vHousingUp * 3.f * fHousingUpScale * (float)(m_iFloor - 1));
			}
			else if (EBuildType::STAIRS == m_wpAttachedHousing.lock()->GetBuildType())
			{
				m_iFloor = m_wpAttachedHousing.lock()->GetFloor() + 1;
				SetPosition(vHousingPos + vHousingUp * 3.f * fHousingUpScale - vHousingRight * 4.f * fHousingRightScale);

				if (m_eBuildType != EBuildType::WALL && m_eBuildType != EBuildType::DOOR)
					--iX;
			}
			else
			{
				SetPosition(vHousingPos - vHousingRight * 4.f * fHousingRightScale);

				if (m_eBuildType != EBuildType::WALL && m_eBuildType != EBuildType::DOOR)
					--iX;
			}

			break;
		}
		case EBuildDir::LEFT_UP:
		{
			if (EBuildType::WALL == m_wpAttachedHousing.lock()->GetBuildType() || EBuildType::DOOR == m_wpAttachedHousing.lock()->GetBuildType())
			{
				m_iFloor = m_wpAttachedHousing.lock()->GetFloor() + 1;
				SetPosition(vHousingPos + vHousingUp * 3.f * fHousingUpScale);
			}
			else
			{
				m_iFloor = m_wpAttachedHousing.lock()->GetFloor() + 1;
				SetPosition(vHousingPos - vHousingRight * 2.f * fHousingRightScale);
			}

			break;
		}
		case EBuildDir::LEFT_DOWN:
		{
			if (EBuildType::WALL == m_wpAttachedHousing.lock()->GetBuildType() || EBuildType::DOOR == m_wpAttachedHousing.lock()->GetBuildType())
			{
				m_iFloor = m_wpAttachedHousing.lock()->GetFloor() - 1;
				SetPosition(vHousingPos - vHousingUp * 3.f * fHousingUpScale);
			}
			else
			{
				m_iFloor = m_wpAttachedHousing.lock()->GetFloor() - 1;
				SetPosition(vHousingPos - vHousingRight * 2.f * fHousingRightScale - 3.f * vHousingUp * fHousingUpScale);
			}

			break;
		}
		}
		break;
	}
	#pragma endregion
	#pragma region UP & DOWN
	case EBuildDir::UP:
	case EBuildDir::DOWN:
	{
		m_iFloor = m_wpAttachedHousing.lock()->GetFloor();
		SetPosition(vHousingPos + Vector3(0.f, 0.001f, 0.f));

		break;
	}
	#pragma endregion
	#pragma region Default
	default:
	{
		SetNoneAttached();
		return;
	}
	#pragma endregion
	}

	m_bAttached = true;
	iFloor = m_iFloor;
	m_tpBuildIndex = tpIndex;
}
EBuildDir Client::CHousingPrefab::DecideBuildDir(const EBuildType& _eBuildType, const EBuildType& _eOpponentBuildType, float _fAngle)
{
	switch (_eBuildType)
	{
		case EBuildType::NORMAL:
		{
			return EBuildDir::UP;
		}
	}

	EBuildDir eBuildDir = EBuildDir::ENUM_END;

	// ※※※ 추후 세부 방향(LOOK_UP, LOOK_DOWN) 결정하는 부분 추가 필요 ※※※
	if ((0.f <= _fAngle && _fAngle < 45.f) || (315.f <= _fAngle && _fAngle < 360.f))
		eBuildDir = EBuildDir::LOOK;
	else if (45.f <= _fAngle && _fAngle < 135.f)
		eBuildDir = EBuildDir::RIGHT;
	else if (135.f <= _fAngle && _fAngle < 225.f)
		eBuildDir = EBuildDir::BACK;
	else if (225.f <= _fAngle && _fAngle < 315.f)
		eBuildDir = EBuildDir::LEFT;

	switch (_eBuildType)
	{
	case EBuildType::WALL:
	case EBuildType::DOOR:
	{
		switch (eBuildDir)
		{
		case EBuildDir::LOOK:
		{
			if (_eOpponentBuildType == EBuildType::FOUNDATION || _eOpponentBuildType == EBuildType::ROOF)
			{
				if (GetPosition().value().y > m_wpAttachedHousing.lock()->GetPosition().value().y)
				{
					return EBuildDir::LOOK_UP;
				}
				else
				{
					return EBuildDir::LOOK_DOWN;
				}
			}
			else if (_eOpponentBuildType == EBuildType::WALL || _eOpponentBuildType == EBuildType::DOOR)
			{
				LookAt(GetPosition().value() + m_wpAttachedHousing.lock()->GetForwardVector().value());

				if (GetPosition().value().y > m_wpAttachedHousing.lock()->GetPosition().value().y + m_wpAttachedHousing.lock()->GetPivotScaleRatio() * 3.f)
				{
					return EBuildDir::LOOK_UP;
				}
				else if (GetPosition().value().y < m_wpAttachedHousing.lock()->GetPosition().value().y - m_wpAttachedHousing.lock()->GetPivotScaleRatio() * 3.f)
				{
					return EBuildDir::LOOK_DOWN;
				}
			}
		}
		case EBuildDir::RIGHT:
		{
			if (_eOpponentBuildType == EBuildType::FOUNDATION || _eOpponentBuildType == EBuildType::ROOF)
			{
				if (GetPosition().value().y > m_wpAttachedHousing.lock()->GetPosition().value().y)
				{
					return EBuildDir::RIGHT_UP;
				}
				else
				{
					return EBuildDir::RIGHT_DOWN;
				}
			}
			else if (_eOpponentBuildType == EBuildType::WALL || _eOpponentBuildType == EBuildType::DOOR)
			{
				LookAt(GetPosition().value() + m_wpAttachedHousing.lock()->GetForwardVector().value());

				if (GetPosition().value().y > m_wpAttachedHousing.lock()->GetPosition().value().y + m_wpAttachedHousing.lock()->GetPivotScaleRatio() * 3.f)
				{
					return EBuildDir::RIGHT_UP;
				}
				else if (GetPosition().value().y < m_wpAttachedHousing.lock()->GetPosition().value().y - m_wpAttachedHousing.lock()->GetPivotScaleRatio() * 3.f)
				{
					return EBuildDir::RIGHT_DOWN;
				}
			}
		}
		case EBuildDir::BACK:
		{
			if (_eOpponentBuildType == EBuildType::FOUNDATION || _eOpponentBuildType == EBuildType::ROOF)
			{
				if (GetPosition().value().y > m_wpAttachedHousing.lock()->GetPosition().value().y)
				{
					return EBuildDir::BACK_UP;
				}
				else
				{
					return EBuildDir::BACK_DOWN;
				}
			}
			else if (_eOpponentBuildType == EBuildType::WALL || _eOpponentBuildType == EBuildType::DOOR)
			{
				LookAt(GetPosition().value() + m_wpAttachedHousing.lock()->GetForwardVector().value());

				if (GetPosition().value().y > m_wpAttachedHousing.lock()->GetPosition().value().y + m_wpAttachedHousing.lock()->GetPivotScaleRatio() * 3.f)
				{
					return EBuildDir::BACK_UP;
				}
				else if (GetPosition().value().y < m_wpAttachedHousing.lock()->GetPosition().value().y - m_wpAttachedHousing.lock()->GetPivotScaleRatio() * 3.f)
				{
					return EBuildDir::BACK_DOWN;
				}
			}
		}
		case EBuildDir::LEFT:
		{
			if (_eOpponentBuildType == EBuildType::FOUNDATION || _eOpponentBuildType == EBuildType::ROOF)
			{
				if (GetPosition().value().y > m_wpAttachedHousing.lock()->GetPosition().value().y)
				{
					return EBuildDir::LEFT_UP;
				}
				else
				{
					return EBuildDir::LEFT_DOWN;
				}
			}
			else if (_eOpponentBuildType == EBuildType::WALL || _eOpponentBuildType == EBuildType::DOOR)
			{
				LookAt(GetPosition().value() + m_wpAttachedHousing.lock()->GetForwardVector().value());

				if (GetPosition().value().y > m_wpAttachedHousing.lock()->GetPosition().value().y + m_wpAttachedHousing.lock()->GetPivotScaleRatio() * 3.f)
				{
					return EBuildDir::LEFT_UP;
				}
				else if(GetPosition().value().y < m_wpAttachedHousing.lock()->GetPosition().value().y - m_wpAttachedHousing.lock()->GetPivotScaleRatio() * 3.f)
				{
					return EBuildDir::LEFT_DOWN;
				}
			}
		}
		default: return eBuildDir;
		}
	}
	case EBuildType::STAIRS:
	{
		if (_eOpponentBuildType == EBuildType::FOUNDATION || _eOpponentBuildType == EBuildType::ROOF)
		{
			return EBuildDir::UP;
		}
		else
		{
			LookAt(GetPosition().value() + m_wpAttachedHousing.lock()->GetForwardVector().value());

			return eBuildDir;
		}
	}
	case EBuildType::FOUNDATION:return eBuildDir;
	case EBuildType::ROOF:		return eBuildDir;
	case EBuildType::SLOPEDROOF:return eBuildDir;
	}

	return eBuildDir;
}

bool Client::CHousingPrefab::IsBuildable()
{
	// 충돌 오브젝트 umap의 크기가 1이상이면 false
	if (!m_umapCollidedGameObjects.empty()) { return false; }

	bool bBuildable = IsBuildable(m_eBuildType);

	return bBuildable;
}
bool Client::CHousingPrefab::IsBuildable(EBuildType _eBuildType)
{
	shared_ptr<CHousingLump> spHousingLump = nullptr;
	shared_ptr<CHousing> spHousing = nullptr;

	// 빌드 타입에 따라 건축 조건을 판단합니다.
	switch (_eBuildType)
	{
	case EBuildType::NORMAL:
	{
		// 1. 지형 위에 있는가?

		return true;
	}
	case EBuildType::FOUNDATION:
	{
		// 2. 거점 안에 있는가?
		if (!m_spBuildManager->IsInBaseCamp(GetPosition().value())) { return false; }

		// 2. 지형 위에 있는가?
		return true;
	}
	case EBuildType::ROOF:
	{
		// 1. 다른 하우징에 부착되어 있는 상태인가?
		if (!m_bAttached) { return false; }

		spHousingLump = m_wpAttachedHousing.lock()->GetHousingLump();
		CHECKF(spHousingLump, L"HousingLump is nullptr : CHousingPrefab");

		// 2. 거점 안에 있는가?
		if (!m_spBuildManager->IsInBaseCamp(GetPosition().value())) { return false; }

		// 3. 하우징 덩어리에 해당 인덱스 자리가 이미 차 있는가?
		spHousing = spHousingLump->FindHousing(m_tpBuildIndex);
		if (spHousing) { return false; }

		// 4. Attach될 상대 하우징에 설치될 방향에 자리가 있는가?
		spHousing = m_wpAttachedHousing.lock()->GetCombinedHousing(m_eBuildDir);
		if (spHousing) { return false; }

		// 5. 지지대 작동 원리를 만족하는가?
		if (!spHousingLump->IsSatisfiedSupportingTheory(m_tpBuildIndex, m_eBuildType)) { return false; }

		return true;
	}
	case EBuildType::STAIRS:
	case EBuildType::SLOPEDROOF:
	case EBuildType::WALL:
	case EBuildType::DOOR:
	{
		// 1. 다른 하우징에 부착되어 있는 상태인가?
		if (!m_bAttached) { return false; }

		spHousingLump = m_wpAttachedHousing.lock()->GetHousingLump();
		CHECKF(spHousingLump, L"HousingLump is nullptr : CHousingPrefab");

		// 2. 거점 안에 있는가?
		if (!m_spBuildManager->IsInBaseCamp(GetPosition().value())) { return false; }

		// 3. 하우징 덩어리에 해당 인덱스 자리가 이미 차 있는가? -> 이 4가지 타입은 인덱스의 영향을 받지 않고 건설
		// 나중에 연결 하우징들을 파괴를 해야 하는 상황이 오면, vecCombinedHousings를 검색하자.
		// spHousing = spHousingLump->FindHousing(m_tpBuildIndex);
		// if (spHousing) { return false; }

		// 4. Attach될 상대 하우징에 설치될 방향에 자리가 있는가?
		spHousing = m_wpAttachedHousing.lock()->GetCombinedHousing(m_eBuildDir);
		if (spHousing) { return false; }

		// 5. 지지대 작동 원리를 만족하는가?
		if (!spHousingLump->IsSatisfiedSupportingTheory(m_tpBuildIndex, m_eBuildType)) { return false; }

		return true;
	}
	}

	return true;
}

void Client::CHousingPrefab::SetNoneAttached()
{
	m_tpBuildIndex = tuple(0, 0, 0);
	m_bAttached = false;
	m_eBuildDir = EBuildDir::ENUM_END;
	m_iFloor = 0;
}
