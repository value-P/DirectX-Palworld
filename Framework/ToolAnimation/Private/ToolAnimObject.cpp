#include "pch.h"
#include "ToolAnimObject.h"

#include "MeshComponent.h"

std::shared_ptr<CToolAnimObject> ToolAnimation::CToolAnimObject::Create(const std::shared_ptr<FToolAnimObjectDesc>& _spPlayerDesc)
{
	shared_ptr<CToolAnimObject> spAnimObject = make_shared<CToolAnimObject>();
	ENSUREF(spAnimObject->Initialize(_spPlayerDesc), L"Failed to initialize CToolAnimObject.");
	return spAnimObject;
}

HRESULT ToolAnimation::CToolAnimObject::Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	if (FAILED(CGameObject::Initialize(_spGameObjectDesc))) { return E_FAIL; }

	InitMeshComponent(static_pointer_cast<FToolAnimObjectDesc>(_spGameObjectDesc));

	return S_OK;
}

HRESULT ToolAnimation::CToolAnimObject::BeginPlay()
{
	return CGameObject::BeginPlay();
}

int32 ToolAnimation::CToolAnimObject::PreTick(float _fDeltaSeconds)
{
	return CGameObject::PreTick(_fDeltaSeconds);
}

int32 ToolAnimation::CToolAnimObject::Tick(float _fDeltaSeconds)
{
	return CGameObject::Tick(_fDeltaSeconds);
}

int32 ToolAnimation::CToolAnimObject::PostTick(float _fDeltaSeconds)
{
	return CGameObject::PostTick(_fDeltaSeconds);
}

HRESULT ToolAnimation::CToolAnimObject::EndPlay()
{
	return CGameObject::EndPlay();
}

void ToolAnimation::CToolAnimObject::Release()
{
	CGameObject::Release();

	if (m_spMeshComponent) { m_spMeshComponent = nullptr; }
}

void ToolAnimation::CToolAnimObject::InitMeshComponent(const std::shared_ptr<FToolAnimObjectDesc>& _spPlayerDesc)
{
	// component desc
	FMeshComponentDesc tBodyMeshComponentDesc;
	tBodyMeshComponentDesc.m_wstrComponentName = L"AnimMeshComponent";
	tBodyMeshComponentDesc.m_eRenderType = ERenderType::RENDER_NBLEND;
	tBodyMeshComponentDesc.m_eComponentType = EComponentType::MESH;
	tBodyMeshComponentDesc.m_wpOwnerGameObject = shared_from_this();

	// mesh component desc
	tBodyMeshComponentDesc.m_wstrModelFilePath = _spPlayerDesc->m_wstrAnimModelPath;
	tBodyMeshComponentDesc.m_wstrShaderName = L"FX_AnimMesh_PNT2TBIBW";
	tBodyMeshComponentDesc.m_matPivot = _spPlayerDesc->m_matPivot;

	m_spMeshComponent = CMeshComponent::Create(make_shared<FMeshComponentDesc>(tBodyMeshComponentDesc));
	AddComponent(tBodyMeshComponentDesc.m_wstrComponentName, m_spMeshComponent);
}
