// default
#include "pch.h"
#include "GameObject.h"

// component
#include "TransformComponent.h"
#include "ColliderComponent.h"

// world
#include "World.h"
#include "Level.h"
#include "Layer.h"

std::shared_ptr<CGameObject> Engine::CGameObject::Create(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	const std::shared_ptr<CGameObject> spGameObject = make_shared<CGameObject>();
	ENSUREF(spGameObject->Initialize(_spGameObjectDesc), L"Failed to initialize gameobject");
	return spGameObject;
}

HRESULT Engine::CGameObject::Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	// gameobject desc
	m_wstrGameObjectName	= _spGameObjectDesc->m_wstrGameObjectName;
	m_bActive				= _spGameObjectDesc->m_bActive;
	m_eMobilityType			= _spGameObjectDesc->m_eMobilityType;

	// transform component desc
	shared_ptr<CTransformComponent::FTransformComponentDesc> spTransformComponentDesc = make_shared<CTransformComponent::FTransformComponentDesc>();
	spTransformComponentDesc->m_wpOwnerGameObject	= shared_from_this();
	spTransformComponentDesc->m_eComponentType		= EComponentType::TRANSFORM;
	spTransformComponentDesc->m_wstrComponentName	= L"TransformComponent";
	spTransformComponentDesc->m_eRenderType			= ERenderType::NONE;
	
	spTransformComponentDesc->m_vLocalScale			= _spGameObjectDesc->m_vInitScale;
	spTransformComponentDesc->m_vLocalRotate		= _spGameObjectDesc->m_vInitRotate;
	spTransformComponentDesc->m_vLocalTranslate		= _spGameObjectDesc->m_vInitPosition;
	
	m_iOctreeIndex									= _spGameObjectDesc->m_iOctreeIndex;
	m_tpOctreeNodeIndex								= _spGameObjectDesc->m_tpOctreeNodeIndex;

	shared_ptr<CTransformComponent> spTransformComponent = CTransformComponent::Create(spTransformComponentDesc);
	AddComponent(spTransformComponentDesc->m_wstrComponentName, spTransformComponent);
	m_wpTransformComponent = spTransformComponent;

	// gameobject ID
	if (!m_iGameObjectID)
	{
		m_iGameObjectID = CWorld::GetInstance()->m_iGameObjectID++;
	}

	return S_OK;
}

HRESULT Engine::CGameObject::BeginPlay()
{
	for (const auto& pair : m_mapComponents)
	{
		if (FAILED(pair.second->BeginPlay())) { return E_FAIL; }
	}

	for (const auto& spChildGameObject : m_vecChildGameObjects)
	{
		if (FAILED(spChildGameObject->BeginPlay())) { return E_FAIL; }
	}

	return S_OK;
}

int32 Engine::CGameObject::PreTick(float _fDeltaSeconds)
{
	for (const auto& pair : m_mapComponents)
	{
		if (!pair.second->IsActive()) { continue; }

		switch (pair.second->PreTickComponent(_fDeltaSeconds))
		{
		case -1:	return -1;
		default:	break;
		}
	}

	for (const auto& spChildGameObject : m_vecChildGameObjects)
	{
		if (!spChildGameObject->IsActive()) { continue; }

		switch (spChildGameObject->PreTick(_fDeltaSeconds))
		{
		case -1:	return -1;
		default:	break;
		}
	}

	return 0;
}

int32 Engine::CGameObject::Tick(float _fDeltaSeconds)
{
	for (const auto& pair : m_mapComponents)
	{
		if (!pair.second->IsActive()) { continue; }

		switch (pair.second->TickComponent(_fDeltaSeconds))
		{
		case -1:	return -1;
		default:	break;
		}
	}

	for (const auto& spChildGameObject : m_vecChildGameObjects)
	{
		if (!spChildGameObject->IsActive()) { continue; }

		switch (spChildGameObject->Tick(_fDeltaSeconds))
		{
		case -1:	return -1;
		default:	break;
		}
	}

	return 0;
}

int32 Engine::CGameObject::PostTick(float _fDeltaSeconds)
{
	for (const auto& pair : m_mapComponents)
	{
		if (!pair.second->IsActive()) { continue; }

		switch (pair.second->PostTickComponent(_fDeltaSeconds))
		{
		case -1:	return -1;
		default:	break;
		}
	}

	for (const auto& spChildGameObject : m_vecChildGameObjects)
	{
		if (!spChildGameObject->IsActive()) { continue; }

		switch (spChildGameObject->PostTick(_fDeltaSeconds))
		{
		case -1:	return -1;
		default:	break;
		}
	}

	return 0;
}

HRESULT Engine::CGameObject::EndPlay()
{
	for (const auto& pair : m_mapComponents)
	{
		if (FAILED(pair.second->EndPlay())) { return E_FAIL; }
	}

	for (const auto& spChildGameObject : m_vecChildGameObjects)
	{
		if (FAILED(spChildGameObject->EndPlay())) { return E_FAIL; }
	}

	return S_OK;
}

void Engine::CGameObject::Release()
{
	// 레이어에 속한 경우, 레이어에서 제외합니다.
	if (!m_wpLayer.expired())
	{
		if (FAILED(m_wpLayer.lock()->RemoveGameObjectFromLayer(m_wstrGameObjectName))) { return; }
	}

	for (auto& pair : m_mapComponents)
	{
		pair.second->Release();
		pair.second = nullptr;
	}
	m_mapComponents.clear();

	for (auto& spChildGameObject : m_vecChildGameObjects)
	{
		spChildGameObject->Release();
		spChildGameObject = nullptr;
	}
	m_vecChildGameObjects.clear();
}

const std::optional<Vector3> Engine::CGameObject::GetForwardVector() const
{
	if (!m_wpTransformComponent.expired())
	{
		return m_wpTransformComponent.lock()->GetForwardVector();
	}
	return nullopt;
}

const std::optional<Vector3> Engine::CGameObject::GetUpVector() const
{
	if (!m_wpTransformComponent.expired())
	{
		return m_wpTransformComponent.lock()->GetUpVector();
	}
	return nullopt;
}

const std::optional<Vector3> Engine::CGameObject::GetRightVector() const
{
	if (!m_wpTransformComponent.expired())
	{
		return m_wpTransformComponent.lock()->GetRightVector();
	}
	return nullopt;
}

const std::optional<Vector3> Engine::CGameObject::GetForwardVectorWithoutY() const
{
	if (!m_wpTransformComponent.expired())
	{
		Vector3 vForward = m_wpTransformComponent.lock()->GetForwardVector();
		vForward.y = 0.f;
		vForward.Normalize();

		return vForward;
	}
	return nullopt;
}

const std::optional<Vector3> Engine::CGameObject::GetRightVectorWithoutY() const
{
	if (!m_wpTransformComponent.expired())
	{
		Vector3 vRight = m_wpTransformComponent.lock()->GetRightVector();
		vRight.y = 0.f;
		vRight.Normalize();

		return vRight;
	}
	return nullopt;
}

const optional<Vector3> Engine::CGameObject::GetScale() const
{
	if (!m_wpTransformComponent.expired())
	{
		return m_wpTransformComponent.lock()->GetScale();
	}
	return nullopt;
}

const std::optional<Vector3> Engine::CGameObject::GetRotate() const
{
	if (!m_wpTransformComponent.expired())
	{
		return m_wpTransformComponent.lock()->GetRotate();
	}
	return nullopt;
}

const std::optional<Quaternion> Engine::CGameObject::GetQuaternion() const
{
	if (!m_wpTransformComponent.expired())
	{
		return m_wpTransformComponent.lock()->GetQuaternion();
	}
	return nullopt;
}

const std::optional<Vector3> Engine::CGameObject::GetPosition() const
{
	if (!m_wpTransformComponent.expired())
	{
		return m_wpTransformComponent.lock()->GetPosition();
	}
	return nullopt;
}

const std::optional<Matrix> Engine::CGameObject::GetTransformMatrix() const
{
	if (!m_wpTransformComponent.expired())
	{
		return m_wpTransformComponent.lock()->GetTransformMatrix();
	}
	return nullopt;
}

void Engine::CGameObject::SetScale(const Vector3& _vLocalScale)
{
	if (m_wpTransformComponent.expired()) { CHECKF(false, L"Transform component is not designated."); return; }
	m_wpTransformComponent.lock()->SetScale(_vLocalScale);
}

void Engine::CGameObject::SetRotate(const Vector3& _vAxis, float _fDegree)
{
	if (m_wpTransformComponent.expired()) { CHECKF(false, L"Transform component is not designated."); return; }
	m_wpTransformComponent.lock()->SetRotate(_vAxis, _fDegree);
}

void Engine::CGameObject::SetRotate(const Vector3& _vRotate)
{
	if (m_wpTransformComponent.expired()) { CHECKF(false, L"Transform component is not designated."); return; }
	m_wpTransformComponent.lock()->SetRotate(_vRotate);
}

void Engine::CGameObject::SetRotateAccumulated(const Vector3& _vAxis, float _fDegree)
{
	if (m_wpTransformComponent.expired()) { CHECKF(false, L"Transform component is not designated."); return; }
	m_wpTransformComponent.lock()->SetRotateAccumulated(_vAxis, _fDegree);
}

void Engine::CGameObject::SetPosition(const Vector3& _vLocalPosition)
{
	if (m_wpTransformComponent.expired()) { CHECKF(false, L"Transform component is not designated."); }
	m_wpTransformComponent.lock()->SetPosition(_vLocalPosition);
}

void Engine::CGameObject::SetTransformMatrix(const Matrix& _matLocal)
{
	if (m_wpTransformComponent.expired()) { CHECKF(false, L"Transform component is not designated."); return; }
	m_wpTransformComponent.lock()->SetTransformMatrix(_matLocal);
}

void Engine::CGameObject::SetTransform(ETransformType _eTransformType, const Vector4& _vTransform)
{
	if (m_wpTransformComponent.expired()) { return; }

	m_wpTransformComponent.lock()->SetTransform(_eTransformType, _vTransform);
}

const std::optional<Vector4> Engine::CGameObject::GetTransform(ETransformType _eTransformType)
{
	if (m_wpTransformComponent.expired()) { return nullopt; }

	return m_wpTransformComponent.lock()->GetTransform(_eTransformType);
}

void Engine::CGameObject::SetGameObjectID()
{
	if (m_iGameObjectID != 0) { return; }

	m_iGameObjectID = CWorld::GetInstance()->m_iGameObjectID++;
}

const std::shared_ptr<CWorld> Engine::CGameObject::GetOwnerWorld() const
{
	return m_wpOwnerWorld.expired() ? nullptr : m_wpOwnerWorld.lock();
}

const std::shared_ptr<CLevel> Engine::CGameObject::GetOwnerLevel() const
{
	return m_wpOwnerLevel.expired() ? nullptr : m_wpOwnerLevel.lock();
}

const std::shared_ptr<CGameObject> Engine::CGameObject::GetParentGameObject() const
{
	return m_wpParentGameObject.expired() ? nullptr : m_wpParentGameObject.lock();
}

const std::shared_ptr<CLayer> Engine::CGameObject::GetLayer() const
{
	return m_wpLayer.expired() ? nullptr : m_wpLayer.lock();
}

void Engine::CGameObject::SetCulled(bool _bCulled)
{
	m_bCulled.store(_bCulled);

	// 게임 오브젝트가 갖고 있는 모든 컴포넌트를 렌더하지 않습니다.
	for (const auto& pair : m_mapComponents)
	{
		pair.second->SetCulled(_bCulled);
	}
}

void Engine::CGameObject::SetActive(bool _bActive)
{
	m_bActive.store(_bActive);

	// 게임 오브젝트가 갖고 있는 모든 컴포넌트를 활성화/비활성화합니다.
	for (const auto& pair : m_mapComponents)
	{
		pair.second->SetActive(_bActive);
	}
}

void Engine::CGameObject::LookAt(const Vector3& _vPosition)
{
	if (m_wpTransformComponent.expired()) { return; }

	m_wpTransformComponent.lock()->LookAt(_vPosition);
}

void Engine::CGameObject::Turn(const Vector3& _vAxis, float _fAngularSpeed)
{
	if (m_wpTransformComponent.expired()) { return; }

	m_wpTransformComponent.lock()->Turn(_vAxis, _fAngularSpeed);
}

void Engine::CGameObject::MoveForward(float _fSpeed)
{
	if (m_wpTransformComponent.expired()) { return; }

	m_wpTransformComponent.lock()->MoveForward(_fSpeed);
}

void Engine::CGameObject::MoveBackward(float _fSpeed)
{
	if (m_wpTransformComponent.expired()) { return; }

	m_wpTransformComponent.lock()->MoveBackward(_fSpeed);
}

void Engine::CGameObject::MoveRight(float _fSpeed)
{
	if (m_wpTransformComponent.expired()) { return; }

	m_wpTransformComponent.lock()->MoveRight(_fSpeed);
}

void Engine::CGameObject::MoveLeft(float _fSpeed)
{
	if (m_wpTransformComponent.expired()) { return; }

	m_wpTransformComponent.lock()->MoveLeft(_fSpeed);
}

void Engine::CGameObject::MoveUp(float _fSpeed)
{
	if (m_wpTransformComponent.expired()) { return; }

	m_wpTransformComponent.lock()->MoveUp(_fSpeed);
}

void Engine::CGameObject::MoveDown(float _fSpeed)
{
	if (m_wpTransformComponent.expired()) { return; }

	m_wpTransformComponent.lock()->MoveDown(_fSpeed);
}

HRESULT Engine::CGameObject::AddComponent(const std::wstring& _wstrComponentName, const std::shared_ptr<CComponent>& _spComponent)
{
	if (m_mapComponents.contains(_wstrComponentName)) { return E_FAIL; }

	m_mapComponents.emplace(_wstrComponentName, _spComponent);
	return S_OK;
}

void Engine::CGameObject::AddChildGameObject(const std::shared_ptr<CGameObject>& _spGameObject)
{
	_spGameObject->SetParentGameObject(shared_from_this());

	m_vecChildGameObjects.emplace_back(_spGameObject);
}

HRESULT Engine::CGameObject::RemoveComponent(const std::wstring& _wstrComponentName)
{
	if (!m_mapComponents.contains(_wstrComponentName)) { return E_FAIL; }

	// 트랜스폼 컴포넌트는 없앨 수 없습니다.
	if (m_mapComponents[_wstrComponentName]->GetComponentType() == EComponentType::TRANSFORM) { return E_FAIL; }

	m_mapComponents[_wstrComponentName]->Release();
	m_mapComponents[_wstrComponentName] = nullptr;
	m_mapComponents.erase(_wstrComponentName);
	return S_OK;
}

void Engine::CGameObject::OnCollisionEnter(CComponent* _pSrcComponent, CComponent* _pDstComponent)
{
}

void Engine::CGameObject::OnCollisionStay(CComponent* _pSrcComponent, CComponent* _pDstComponent)
{
}

void Engine::CGameObject::OnCollisionExit(CComponent* _pSrcComponent, CComponent* _pDstComponent)
{
}

void Engine::CGameObject::OnTriggerEnter(CComponent* _pSrcComponent, CComponent* _pDstComponent)
{
}

void Engine::CGameObject::OnTriggerStay(CComponent* _pSrcComponent, CComponent* _pDstComponent)
{
}

void Engine::CGameObject::OnTriggerExit(CComponent* _pSrcComponent, CComponent* _pDstComponent)
{
}

void Engine::CGameObject::OnAnimEnter()
{
	return;
}

void Engine::CGameObject::OnAnimStay()
{
	return;
}

void Engine::CGameObject::OnAnimExit()
{
	return;
}

void Engine::CGameObject::AnimNotifyCall(const list<FAnimNotifyData>& _lstAnimNotify)
{
	return;
}

void Engine::CGameObject::OnTimerOver(const shared_ptr<CTimer>& _spTimer)
{
	return;
}
