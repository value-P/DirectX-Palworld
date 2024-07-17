#include "pch.h"
#include "RectComponent.h"

#include "EngineModule.h"
#include "PipelineManager.h"
#include "RenderManager.h"
#include "InputManager.h"
#include "CameraManager.h"
#include "ShaderHandler.h"
#include "PointBuffer.h"
#include "EffectShader.h"

#include "Panel.h"

#include "EffectShader.h"
#include "TextureHandler.h"
#include "TextComponent.h"

std::shared_ptr<CRectComponent> Engine::CRectComponent::Create(const std::shared_ptr<FRectComponentDesc>& _spRectComponentDesc)
{
	const std::shared_ptr<CRectComponent> spRectComponent = make_shared<CRectComponent>();
	ENSUREF(spRectComponent->InitializeComponent(_spRectComponentDesc), L"Failed to initialize rect component.");
	return spRectComponent;
}

HRESULT Engine::CRectComponent::InitializeComponent(const std::shared_ptr<FComponentDesc>& _spComponentDesc)
{
	// component desc
	if (FAILED(CComponent::InitializeComponent(_spComponentDesc))) { return E_FAIL; }

	// rect component desc
	std::shared_ptr<FRectComponentDesc> spRectComponentDesc = static_pointer_cast<FRectComponentDesc>(_spComponentDesc);
	m_wstrShaderFileName = spRectComponentDesc->m_wstrShaderFileName;
	m_iPassIndex = spRectComponentDesc->m_iPassIndex;
	m_iDepth = spRectComponentDesc->m_iDepth;
	m_eUIType = spRectComponentDesc->m_eUIType;
	m_vColor = spRectComponentDesc->m_vColor;
	m_fAlpha = spRectComponentDesc->m_fAlpha;
	m_fRectAngle = spRectComponentDesc->m_fRectAngle;
	m_bMask = spRectComponentDesc->m_bMask;
	m_bAlpha = spRectComponentDesc->m_bAlpha;
	m_bColor = spRectComponentDesc->m_bColor;
	m_bInteraction = spRectComponentDesc->m_bInteraction;

	m_wpOwnerGameObject = spRectComponentDesc->m_wpOwnerGameObject;

	// etc
	m_spPointBuffer = CPointBuffer::Create();
	m_wpEffectShader = CShaderHandler::GetInstance()->FindEffectShader(m_wstrShaderFileName);

	m_wstrMaskTextureFileName = spRectComponentDesc->m_wstrMaskTextureFileName;

	// SRV : Mask�� 0�� ������ ����
	m_vecSRVs.push_back(CTextureHandler::GetInstance()->FindShaderResourceView(m_wstrMaskTextureFileName, ETextureType::UI).Get());

	if (!m_wpEffectShader.expired())
	{
		// Bind Variables �������� ������
		m_cpVariable_UiInfo = m_wpEffectShader.lock()->GetRawVariableByName("g_tUiInfo");
		m_cpVariable_Textures = m_wpEffectShader.lock()->GetShaderResourceVariableByName("g_Textures");
	}

	return S_OK;
}

HRESULT Engine::CRectComponent::BeginPlay()
{
	return S_OK;
}

int32 Engine::CRectComponent::PreTickComponent(float _fDeltaSeconds)
{
	m_matView = Matrix::Identity;
	m_matProjection = XMMatrixOrthographicLH(1280.0f, 720.0f, 1.0f, 0.0f);

	return 0;
}

int32 Engine::CRectComponent::TickComponent(float _fDeltaSeconds)
{
	m_fRectDeltaSeconds = _fDeltaSeconds;
	return 0;
}

int32 Engine::CRectComponent::PostTickComponent(float _fDeltaSeconds)
{
	// ���� ������ �������� Rect ����ü�� ä���ݴϴ�.
	m_tRect =
	{
		static_cast<LONG>(m_vCenter.x - (GetRectScale().x * 0.5f)),
		static_cast<LONG>(m_vCenter.y - (GetRectScale().y * 0.5f)),
		static_cast<LONG>(m_vCenter.x + (GetRectScale().x * 0.5f)),
		static_cast<LONG>(m_vCenter.y + (GetRectScale().y * 0.5f)),
	};

	if (EProjectionType::ORTHOGONAL == dynamic_pointer_cast<CUI>(m_wpOwnerGameObject.lock())->GetUIProjectionType() && EUIType::DYNAMIC_IMAGE != m_eUIType)
	{
		// �ǹ� ������Ʈ�� ��� �������� ���� �ƴ� ������Ʈ�� ��� �����ǥ�� ���θ� üũ�� �� �ٲ��ݴϴ�.
		if (shared_from_this() == static_pointer_cast<CPanel>(m_wpOwnerGameObject.lock())->GetPivotComponent() ||
			nullptr == static_pointer_cast<CPanel>(m_wpOwnerGameObject.lock())->GetPivotComponent() ||
			false == m_bRelativeCenter)
		{
			SetRectTransform(ETransformType::POSITION, Vector4(m_vCenter.x - 1280.f * 0.5f, -m_vCenter.y + 720.f * 0.5f, 0.0f, 1.f));
		}
		// �ǹ� ������Ʈ�� �ƴϸ鼭 �����ǥ�� ���ٰ� ������ �����ǥ�������� ���� ��ǥ�� ���� �ٲ��ݴϴ�.
		else if (true == m_bRelativeCenter && shared_from_this() != static_pointer_cast<CPanel>(m_wpOwnerGameObject.lock())->GetPivotComponent())
		{
			CalculateOrthgonalScreenRelativeCenter();
			SetRectTransform(ETransformType::POSITION, Vector4(m_vCenter.x - 1280.f * 0.5f, -m_vCenter.y + 720.f * 0.5f, 0.0f, 1.f));
		}
		// RectAngle ���� �������� matRotation�� ����� �����ŵ�ϴ�.
		Matrix matRotation = ChangeRotationMatrix(m_fRectAngle);
		SetRectRotate(matRotation);
	}
	else if (EProjectionType::PERSPECTIVE == dynamic_pointer_cast<CUI>(m_wpOwnerGameObject.lock())->GetUIProjectionType())
	{
		// RectAngle ���� �������� matRotation�� ����� �����ŵ�ϴ�.
		Matrix matRotation = ChangeRotationMatrix(m_fRectAngle);
		SetRectRotate(matRotation);

		// ���� ���� ���� �ӽ� �ڵ��Դϴ�.
		if (shared_from_this() != static_pointer_cast<CPanel>(m_wpOwnerGameObject.lock())->GetPivotComponent())
		{
			if (EUIType::DYNAMIC_IMAGE != m_eUIType)
			{
				CalculateScreenRelativeCenterToCenter();
			}
		}
		else
		{
			// ���� ����Դϴ�.
			if (m_bTool)
			{
				m_vWorldToScreenPosition = m_vCenter;
			}
			// Ŭ���̾�Ʈ�� ����Դϴ�. 
			else
			{
				m_vWorldToScreenPosition = CalculateViewportPosition(GetRectPosition());
			}
		}
	}

	CEngineModule::GetInstance()->GetRenderManager()->AddComponent(shared_from_this(), m_eRenderType);

	return 0;
}

HRESULT Engine::CRectComponent::EndPlay()
{
	return S_OK;
}

void Engine::CRectComponent::Render(int32 _iShaderPath)
{
	if (m_wstrShaderFileName.empty()) { return; }

	BindVariable();
	m_wpEffectShader.lock()->BeginPass(m_iPassIndex);

	m_spPointBuffer->Render();
}

void Engine::CRectComponent::Release()
{
	m_cpVariable_UiInfo = nullptr;
	m_cpVariable_Textures = nullptr;

	m_vecSRVs.clear();

	m_spPointBuffer->Release();
	m_spPointBuffer = nullptr;
}

HRESULT Engine::CRectComponent::BindVariable()
{
	//if (EProjectionType::ORTHOGONAL == dynamic_pointer_cast<CUI>(m_wpOwnerGameObject.lock())->GetUIProjectionType() && EUIType::DYNAMIC_IMAGE != m_eUIType)
	//{
	//	// �ǹ� ������Ʈ�� ��� �������� ���� �ƴ� ������Ʈ�� ��� �����ǥ�� ���θ� üũ�� �� �ٲ��ݴϴ�.
	//	if (shared_from_this() == static_pointer_cast<CPanel>(m_wpOwnerGameObject.lock())->GetPivotComponent() ||
	//		nullptr == static_pointer_cast<CPanel>(m_wpOwnerGameObject.lock())->GetPivotComponent() ||
	//		false == m_bRelativeCenter)
	//	{
	//		SetRectTransform(ETransformType::POSITION, Vector4(m_vCenter.x - 1280.f * 0.5f, -m_vCenter.y + 720.f * 0.5f, 0.0f, 1.f));
	//	}
	//	// �ǹ� ������Ʈ�� �ƴϸ鼭 �����ǥ�� ���ٰ� ������ �����ǥ�������� ���� ��ǥ�� ���� �ٲ��ݴϴ�.
	//	else if (true == m_bRelativeCenter && shared_from_this() != static_pointer_cast<CPanel>(m_wpOwnerGameObject.lock())->GetPivotComponent())
	//	{
	//		CalculateOrthgonalScreenRelativeCenter();
	//		SetRectTransform(ETransformType::POSITION, Vector4(m_vCenter.x - 1280.f * 0.5f, -m_vCenter.y + 720.f * 0.5f, 0.0f, 1.f));
	//	}
	//	// RectAngle ���� �������� matRotation�� ����� �����ŵ�ϴ�.
	//	Matrix matRotation = ChangeRotationMatrix(m_fRectAngle);
	//	SetRectRotate(matRotation);
	//}
	//else if (EProjectionType::PERSPECTIVE == dynamic_pointer_cast<CUI>(m_wpOwnerGameObject.lock())->GetUIProjectionType())
	//{
	//	// RectAngle ���� �������� matRotation�� ����� �����ŵ�ϴ�.
	//	Matrix matRotation = ChangeRotationMatrix(m_fRectAngle);
	//	SetRectRotate(matRotation);

	//	// ���� ���� ���� �ӽ� �ڵ��Դϴ�.
	//	if (shared_from_this() != static_pointer_cast<CPanel>(m_wpOwnerGameObject.lock())->GetPivotComponent())
	//	{
	//		if (EUIType::DYNAMIC_IMAGE != m_eUIType)
	//		{
	//			CalculateScreenRelativeCenterToCenter();
	//		}
	//	}
	//	else
	//	{
	//		// ���� ����Դϴ�.
	//		if (m_bTool)
	//		{
	//			m_vWorldToScreenPosition = m_vCenter;
	//		}
	//		// Ŭ���̾�Ʈ�� ����Դϴ�. 
	//		else
	//		{
	//			m_vWorldToScreenPosition = CalculateViewportPosition(GetRectPosition());
	//		}
	//	}
	//}

	if (EProjectionType::ORTHOGONAL == dynamic_pointer_cast<CUI>(m_wpOwnerGameObject.lock())->GetUIProjectionType())
	{
		m_tUiInfo.matWorld = m_matRectWorld;
	}
	else if (EProjectionType::PERSPECTIVE == dynamic_pointer_cast<CUI>(m_wpOwnerGameObject.lock())->GetUIProjectionType())
	{
		Vector4 vPosition = Vector4(m_vWorldToScreenPosition.x - 1280.f * 0.5f, -m_vWorldToScreenPosition.y + 720.f * 0.5f, 0.0f, 1.f);
		Matrix matWorld = m_matRectWorld;
		matWorld._41 = vPosition.x;
		matWorld._42 = vPosition.y;
		matWorld._43 = vPosition.z;
		matWorld._44 = 1.f;

		m_tUiInfo.matWorld = matWorld;
	}

	m_tUiInfo.matView = m_matView;
	m_tUiInfo.matProj = m_matProjection;
	m_tUiInfo.fRatio = m_fRatio;
	m_tUiInfo.bAlpha = m_bAlpha;
	m_tUiInfo.bColor = m_bColor;
	m_tUiInfo.bMask = m_bMask;
	m_tUiInfo.fAlpha = m_fAlpha;
	m_tUiInfo.vColor = m_vColor;

	ENSURE(m_cpVariable_UiInfo->SetRawValue(&m_tUiInfo, 0, sizeof(m_tUiInfo)));
	ENSURE(m_cpVariable_Textures->SetResourceArray(m_vecSRVs.data(), 0, (int32)m_vecSRVs.size()));

	return S_OK;
}

bool Engine::CRectComponent::IsInMouse()
{
	// ���콺�� ��ũ�� ��ǥ�� �ҷ��ɴϴ�.
	pair<int32, int32> prMouse = CEngineModule::GetInstance()->GetInputManager()->GetCursorScreenPosition();

	// ���콺�� ��ǥ�� Rect �ȿ� �ִ� �� �Ǵ��մϴ�.
	if (m_tRect.left <= prMouse.first && m_tRect.right >= prMouse.first
		&& m_tRect.top <= prMouse.second && m_tRect.bottom >= prMouse.second)
	{
		return true;
	}

	// ���콺�� ��ǥ�� Rect �ȿ� ������ false�� ��ȯ�մϴ�.
	return false;
}

void Engine::CRectComponent::AddMouseMove()
{
	pair<int32, int32> prMouse = CEngineModule::GetInstance()->GetInputManager()->GetCursorScreenPosition();

	if (!m_bMouseReset)
	{
		m_prPreMouse = prMouse;
		m_bMouseReset = true;
	}

	pair<int32, int32> prMovementMouse = { prMouse.first - m_prPreMouse.first, prMouse.second - m_prPreMouse.second };
	// ���� �������� ���콺 �̵����� �����ݴϴ�.
	m_vCenter.x = m_vCenter.x + prMovementMouse.first;
	m_vCenter.y = m_vCenter.y + prMovementMouse.second;


	if (EUIType::TEXT == static_pointer_cast<CRectComponent>(shared_from_this())->GetUIType())
	{
		static_pointer_cast<CTextComponent>(shared_from_this())->SetScreenPosition(m_vCenter);
	}
	else
	{
		// ���� ���ο� ���� ������ �������� RECT ����ü�� ������Ʈ���ݴϴ�.
		m_tRect =
		{
			static_cast<LONG>(m_vCenter.x - (GetRectScale().x * 0.5f)),
			static_cast<LONG>(m_vCenter.y - (GetRectScale().y * 0.5f)),
			static_cast<LONG>(m_vCenter.x + (GetRectScale().x * 0.5f)),
			static_cast<LONG>(m_vCenter.y + (GetRectScale().y * 0.5f)),
		};
	}

	if (EProjectionType::ORTHOGONAL == dynamic_pointer_cast<CUI>(m_wpOwnerGameObject.lock())->GetUIProjectionType())
	{
		if (shared_from_this() != static_pointer_cast<CPanel>(m_wpOwnerGameObject.lock())->GetPivotComponent() &&
			true == m_bRelativeCenter && nullptr != static_pointer_cast<CPanel>(m_wpOwnerGameObject.lock())->GetPivotComponent())
		{
			CenterToRelativeCenter();
		}
		else
		{
			SetRectTransform(ETransformType::POSITION, Vector4(m_vCenter.x - 1280.f * 0.5f, -m_vCenter.y + 720.f * 0.5f, 0.1f, 1.f));
		}
	}
	else if (EProjectionType::PERSPECTIVE == dynamic_pointer_cast<CUI>(m_wpOwnerGameObject.lock())->GetUIProjectionType() && true == m_bTool)
	{
		if (shared_from_this() != static_pointer_cast<CPanel>(m_wpOwnerGameObject.lock())->GetPivotComponent() &&
			true == m_bRelativeCenter && nullptr != static_pointer_cast<CPanel>(m_wpOwnerGameObject.lock())->GetPivotComponent())
		{
			CenterToRelativeCenter();
		}
	}

	m_prPreMouse = prMouse;
}

void Engine::CRectComponent::CheckClickInRect()
{
	if (m_bFocusedControl == false && CEngineModule::GetInstance()->GetInputManager()->IsMouseDown(EMouseActionType::LEFT_BUTTON))
	{
		if (IsInMouse())
		{
			m_bFocusedControl = true;
		}
	}
}

void Engine::CRectComponent::CheckMouseDown()
{
	if (CEngineModule::GetInstance()->GetInputManager()->IsMouseDown(EMouseActionType::RIGHT_BUTTON))
	{
		if (IsInMouse())
		{
			SetPreMouse(CEngineModule::GetInstance()->GetInputManager()->GetCursorScreenPosition());
		}
	}
}

Matrix Engine::CRectComponent::ChangeRotationMatrix(float _fDegree)
{
	return XMMatrixRotationAxis(Vector4(0.0f, 0.0f, 1.0f, 0.0f), XMConvertToRadians(_fDegree));
}

Vector2 Engine::CRectComponent::CalculateViewportPosition(Vector3 _vPosition)
{
	// ���� ���� ��ǥ�� �����ݴϴ�.
	//Vector4 vPosition = GetRectTransform(ETransformType::POSITION);//Vector3(GetRectTransform(ETransformType::POSITION).x, GetRectTransform(ETransformType::POSITION).y, GetRectTransform(ETransformType::POSITION).z);
	Vector4 vPosition = Vector4(_vPosition.x, _vPosition.y, _vPosition.z, 1.0f);
	Matrix matView = CEngineModule::GetInstance()->GetCameraManager()->GetCurViewMatrix();
	Matrix matProjection = CEngineModule::GetInstance()->GetCameraManager()->GetCurPerspectiveProjectionMatrix();

	// ���� ���� ��ǥ�� �������� �佺���̽� ��ǥ�� ��ȯ���ݴϴ�.
	Vector4 vViewPosition = XMVector3TransformCoord(vPosition, matView);

	if (vViewPosition.z < 0.0f)
	{
		return Vector2(-3000.0f, -3000.0f);
	}

	Vector4 vProjectionPosition = XMVector3TransformCoord(vViewPosition, matProjection);
	Vector2 vScreenPosition;

	vScreenPosition.x = ((vProjectionPosition.x + 1.0f) / 2.f) * 1280.f;
	vScreenPosition.y = ((-vProjectionPosition.y + 1.0f) / 2.f) * 720.0f;

	return vScreenPosition;
}

// �� ���Ϳ��� �ǹ�������Ʈ�� ���͸� ���� �����ǥ�� �����ݴϴ�.
void Engine::CRectComponent::CenterToRelativeCenter()
{
	// ������ǥ�� �ҷ��ɴϴ�.
	shared_ptr<CRectComponent> spComponent = dynamic_pointer_cast<CPanel>(m_wpOwnerGameObject.lock())->GetPivotComponent();
	if (nullptr != spComponent)
	{
		// ������ǥ�� �����ǥ�� ��ȯ�մϴ�.
		m_vScreenRelativeCenter.x = m_vCenter.x - spComponent->GetCenterPosition().x;
		m_vScreenRelativeCenter.y = m_vCenter.y - spComponent->GetCenterPosition().y;
	}
}

void Engine::CRectComponent::WorldToRelativeCenter()
{
	// ������ǥ�� �ҷ��ɴϴ�.
	shared_ptr<CRectComponent> spComponent = dynamic_pointer_cast<CPanel>(m_wpOwnerGameObject.lock())->GetPivotComponent();
	if (nullptr != spComponent)
	{
		// ������ǥ�� �����ǥ�� ��ȯ�մϴ�.
		// ������ǥ ����
		m_vWorldRelativeCenter.x = GetRectPosition().x - spComponent->GetRectPosition().x;
		m_vWorldRelativeCenter.y = GetRectPosition().y - spComponent->GetRectPosition().y;
		m_vWorldRelativeCenter.z = GetRectPosition().z - spComponent->GetRectPosition().z;
	}
}

void Engine::CRectComponent::SortRelativeCenterToCenter()
{
	// ������ǥ�� �ҷ��ɴϴ�.
	shared_ptr<CRectComponent> spComponent = dynamic_pointer_cast<CPanel>(m_wpOwnerGameObject.lock())->GetPivotComponent();
	// �����ǥ�� ������ǥ�� ��ȯ�մϴ�.
	if (nullptr != spComponent)
	{
		Matrix matProjection = CEngineModule::GetInstance()->GetCameraManager()->GetCurPerspectiveProjectionMatrix();

		Vector3 vPos;
		vPos.x = m_vWorldRelativeCenter.x + spComponent->GetRectPosition().x;
		vPos.y = m_vWorldRelativeCenter.y + spComponent->GetRectPosition().y;
		vPos.z = m_vWorldRelativeCenter.z + spComponent->GetRectPosition().z;

		SetRectPosition(vPos);
	}
}

Vector3 Engine::CRectComponent::SortCameraRelativeToCenter()
{
	// ������ǥ�� �ҷ��ɴϴ�.
	shared_ptr<CRectComponent> spComponent = dynamic_pointer_cast<CPanel>(m_wpOwnerGameObject.lock())->GetPivotComponent();

	// �����ǥ�� ������ǥ�� ��ȯ�մϴ�.
	if (nullptr != spComponent)
	{
		Vector3 vResultPos;
		Vector3 vPos = spComponent->GetRectPosition();

		Vector3 vLook = CEngineModule::GetInstance()->GetCameraManager()->GetCurCameraWorldForwardVector();
		Vector3 vUp = CEngineModule::GetInstance()->GetCameraManager()->GetCurCameraWorldUpVector();//Vector3::UnitY;
		Vector3 vRight = CEngineModule::GetInstance()->GetCameraManager()->GetCurCameraWorldRightVector();//vLook.Cross(vUp);

		// ī�޶� �������� ��� ��ġ�� ���� ��ġ�� �ٲ��ݴϴ�.
		vPos += vRight * m_vWorldRelativeCenter.x;
		vPos += vUp * m_vWorldRelativeCenter.y;
		vPos += vLook * m_vWorldRelativeCenter.z;

		return vPos;
	}
	return Vector3();
}

void Engine::CRectComponent::CalculateOrthgonalScreenRelativeCenter()
{
	// ������ǥ�� �ҷ��ɴϴ�.
	shared_ptr<CRectComponent> spComponent = dynamic_pointer_cast<CPanel>(m_wpOwnerGameObject.lock())->GetPivotComponent();
	// �����ǥ�� ������ǥ�� ��ȯ�մϴ�.
	if (nullptr != spComponent)
	{
		// ���� ������ǥ�� �ƴҶ� �����մϴ�.
		if (shared_from_this() != spComponent)
		{
			// ���ο� ��ǥ�� ������ �� �׳� ���Ϳ� ��������
			m_vCenter.x = m_vScreenRelativeCenter.x + spComponent->GetCenterPosition().x;
			m_vCenter.y = m_vScreenRelativeCenter.y + spComponent->GetCenterPosition().y;
		}
	}
}

void Engine::CRectComponent::CalculateScreenRelativeCenterToCenter()
{
	// ������ǥ�� �ҷ��ɴϴ�.
	shared_ptr<CRectComponent> spComponent = dynamic_pointer_cast<CPanel>(m_wpOwnerGameObject.lock())->GetPivotComponent();
	// �����ǥ�� ������ǥ�� ��ȯ�մϴ�.
	if (nullptr != spComponent)
	{
		// ���� ������ǥ�� �ƴҶ� �����մϴ�.
		if (shared_from_this() != spComponent)
		{
			m_vWorldToScreenPosition.x = m_vScreenRelativeCenter.x + spComponent->GetWorldToScreenPosition().x;
			m_vWorldToScreenPosition.y = m_vScreenRelativeCenter.y + spComponent->GetWorldToScreenPosition().y;
		}
	}
}

void Engine::CRectComponent::SetMaskTextureFileName(const std::wstring& _wstrMaskTextureName)
{
	if (m_vecSRVs.size() == 2)
	{
		m_vecSRVs[0] = nullptr;
	}

	m_vecSRVs[0] = CTextureHandler::GetInstance()->FindShaderResourceView(_wstrMaskTextureName, ETextureType::UI).Get();
	m_wstrMaskTextureFileName = _wstrMaskTextureName;
}

void Engine::CRectComponent::SetWorldtoCenterPosition(const Vector4& _vPosition)
{
	m_vCenter.x = _vPosition.x + 1280.f * 0.5f;
	m_vCenter.y = -_vPosition.y + 720.f * 0.5f;
}

Vector3 Engine::CRectComponent::GetRectWorldPosition(Vector3 _vCenter)
{
	return Vector3(m_vCenter.x - 1280.f * 0.5f, -m_vCenter.y + 720.f * 0.5f, 0.0f);
}

void Engine::CRectComponent::SetRectScale(const Vector3& _vScale)
{
	Vector3 vScale = _vScale;
	if (0.0f == _vScale.x) { vScale.x = 0.2f; }
	if (0.0f == _vScale.y) { vScale.y = 0.2f; }
	if (0.0f == _vScale.z) { vScale.z = 0.2f; }

	SetRectTransform(ETransformType::RIGHT, XMVector3Normalize(GetRectTransform(ETransformType::RIGHT)) * vScale.x);
	SetRectTransform(ETransformType::UP, XMVector3Normalize(GetRectTransform(ETransformType::UP)) * vScale.y);
	SetRectTransform(ETransformType::FORWARD, XMVector3Normalize(GetRectTransform(ETransformType::FORWARD)) * vScale.z);
}

void Engine::CRectComponent::SetRectRotate(const Vector3& _vAxis, float _fDegree)
{
	Vector4 v4Axis = { _vAxis.x, _vAxis.y, _vAxis.z, 0.0f };

	Matrix matRotate = XMMatrixRotationAxis(v4Axis, XMConvertToRadians(_fDegree));
	Vector3 vScale = GetRectScale();

	for (size_t iIndex = 0; iIndex < 3; ++iIndex)
	{
		Vector4 vTemp = Vector4::Zero;

		*((float*)&vTemp + iIndex) = 1.0f * *((float*)&vScale + iIndex);

		SetRectTransform(static_cast<ETransformType>(iIndex), XMVector4Transform(vTemp, matRotate));
	}
}


void Engine::CRectComponent::SetRectRotate(const Vector4& _vRotation)
{
	Matrix matRotation = XMMatrixRotationQuaternion(_vRotation);

	// ���� �������� �����մϴ�.
	Vector3 vScale = GetRectScale();

	for (int32 i = 0; i < 3; i++)
	{
		Vector4 vTemp = Vector4::Zero;

		// �������� �����ϸ鼭 �����մϴ�.
		*((float*)&vTemp + i) = 1.f * *((float*)&vScale + i);

		SetRectTransform(ETransformType(i), XMVector4Transform(vTemp, matRotation));
	}
}

void Engine::CRectComponent::SetRectRotate(const Matrix& _matRotation)
{
	// ���� �������� �����մϴ�.
	Vector3 vScale = GetRectScale();

	for (int32 i = 0; i < 3; i++)
	{
		Vector4 vTemp = Vector4::Zero;

		// �������� �����ϸ鼭 �����մϴ�.
		*((float*)&vTemp + i) = 1.f * *((float*)&vScale + i);

		SetRectTransform(ETransformType(i), XMVector4Transform(vTemp, _matRotation));
	}
}

void Engine::CRectComponent::SetRectPosition(const Vector3& _vTranlate)
{
	Vector4 v4Translate = { _vTranlate.x, _vTranlate.y, _vTranlate.z, 1.0f };
	SetRectTransform(ETransformType::POSITION, v4Translate);
}

void Engine::CRectComponent::SetWorldMatrix(const Matrix& _WorldMatrix)
{
	m_matRectWorld = _WorldMatrix;
}

Vector3 Engine::CRectComponent::GetRectScale()
{
	return Vector3
	(
		XMVector3Length(GetRectTransform(ETransformType::RIGHT)).m128_f32[0],
		XMVector3Length(GetRectTransform(ETransformType::UP)).m128_f32[0],
		XMVector3Length(GetRectTransform(ETransformType::FORWARD)).m128_f32[0]
	);
}

Vector3 Engine::CRectComponent::GetRectRotate()
{
	Vector3 vScale = Vector3::One;
	Quaternion vQuaternion = Quaternion::Identity;
	Vector3 vPosition = Vector3::Zero;

	m_matRectWorld.Decompose(vScale, vQuaternion, vPosition);
	return vQuaternion.ToEuler();
}

Vector3 Engine::CRectComponent::GetRectPosition()
{
	return m_matRectWorld.Translation();
}

Matrix Engine::CRectComponent::GetWorldMatrix()
{
	return m_matRectWorld;
}

void Engine::CRectComponent::SetRectTransform(ETransformType _eTransformType, const Vector4& _vTransform)
{
	::memcpy(&m_matRectWorld.m[(int32)_eTransformType], &_vTransform, sizeof(Vector4));
}

Vector4 Engine::CRectComponent::GetRectTransform(ETransformType _eTransformType) const
{
	Vector4 vTemp = Vector4::Zero;
	::memcpy(&vTemp, &m_matRectWorld.m[(int32)_eTransformType], sizeof(Vector4));
	return vTemp;
}
