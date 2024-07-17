#include "pch.h"
#include "Panel.h"

#include "RectComponent.h"
#include "ImageComponent.h"
#include "ButtonComponent.h"
#include "SliderComponent.h"
#include "ProgressBarComponent.h"
#include "AnimationImageComponent.h"
#include "TextComponent.h"

#include "TransformComponent.h"

#include "StringUtility.h"

std::shared_ptr<CPanel> Engine::CPanel::Create(const std::shared_ptr<FPanelDesc>& _spPanelDesc)
{
	const std::shared_ptr<CPanel> spPanel = make_shared<CPanel>();
	ENSUREF(spPanel->Initialize(_spPanelDesc), L"Failed to initialize panel.");
	return spPanel;
}

std::shared_ptr<CPanel> Engine::CPanel::CreateFromJson(const std::wstring& _wstrJsonFileName)
{
	const std::shared_ptr<CPanel> spPanel = make_shared<CPanel>();

	string strJsonFileName = CStringUtility::ToString(_wstrJsonFileName.c_str());
	ifstream ifsFilePath("../../Resource/UIs/" + strJsonFileName + ".json");
	if (!ifsFilePath.is_open())
	{
		return nullptr;
	}

	string strJsonFile((istreambuf_iterator<char>(ifsFilePath)), (istreambuf_iterator<char>()));

	rapidjson::Document doc;
	doc.Parse(strJsonFile.c_str());

	// ������������ �г��ϳ��� ������ �ε��մϴ�.
	int32 iPanelIndex = 0;
	spPanel->LoadJson(spPanel, doc, iPanelIndex);
	iPanelIndex++;

	rapidjson::Value& iPanelSize = doc["Child Panel Size"];

	// ���� �ڽ� �г��� ����� ������ �� �޾ƿͼ� ���� ��� for���� ���� �г��� ������ְ� AddChildGameObject() ���ݴϴ�.
	for (int32 i = 0; i < iPanelSize.GetInt(); i++)
	{
		const std::shared_ptr<CPanel> spChildPanel = make_shared<CPanel>();
		if (nullptr != spPanel->GetPivotComponent())
		{
			spChildPanel->SetPivotComponent(spPanel->GetPivotComponent());
		}
		spChildPanel->LoadJson(spChildPanel, doc, iPanelIndex);
		// �� �ε��� ������ ���� �ڽĿ�����Ʈ�� �Ҽӵǰ� ���ݴϴ�.
		spPanel->AddChildGameObject(spChildPanel);
		iPanelIndex++;
	}

	return spPanel;
}

HRESULT Engine::CPanel::Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	// ui desc
	if (FAILED(CUI::Initialize(_spGameObjectDesc))) { return E_FAIL; }

	// panel desc
	shared_ptr<FPanelDesc> spPanelDesc = static_pointer_cast<FPanelDesc>(_spGameObjectDesc);

	return S_OK;
}

HRESULT Engine::CPanel::BeginPlay()
{
	if (FAILED(CUI::BeginPlay())) { return E_FAIL; }

	return S_OK;
}

int32 Engine::CPanel::PreTick(float _fDeltaSeconds)
{
	if (!m_bActive) { return 0; }
	switch (CUI::PreTick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return 0;
}

int32 Engine::CPanel::Tick(float _fDeltaSeconds)
{
	if (!m_bActive) { return 0; }
	switch (CUI::Tick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return 0;
}

int32 Engine::CPanel::PostTick(float _fDeltaSeconds)
{
	if (!m_bActive) { return 0; }
	switch (CUI::PostTick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return 0;
}

HRESULT Engine::CPanel::EndPlay()
{
	if (FAILED(CUI::EndPlay())) { return E_FAIL; }

	return S_OK;
}

void Engine::CPanel::Release()
{
	m_spPivotComponent = nullptr;

	CUI::Release();
}

std::shared_ptr<CRectComponent> Engine::CPanel::GetFindSelectRectComponent()
{
	std::shared_ptr<CRectComponent> spRectComponent = nullptr;

	// �ӽ÷� ��� ��Ʈ ������Ʈ��� �����մϴ�.
	for (auto& pair : m_mapComponents)
	{
		if (TEXT("TransformComponent") == pair.second->GetComponentName()) { continue; }

		dynamic_pointer_cast<CRectComponent>(pair.second)->CheckClickInRect();
		dynamic_pointer_cast<CRectComponent>(pair.second)->CheckMouseDown();

		if (dynamic_pointer_cast<CRectComponent>(pair.second)->IsInMouse())
		{
			// ��Ʈ���� Depth ���� Ȯ���Ͽ� ���� ���� ���� ���� ������ �� �ֵ��� �մϴ�.
			if (spRectComponent == nullptr || spRectComponent->GetDepth() > dynamic_pointer_cast<CRectComponent>(pair.second)->GetDepth())
			{
				spRectComponent = dynamic_pointer_cast<CRectComponent>(pair.second);
			}
		}
	}

	return spRectComponent;
}

std::shared_ptr<CGameObject> Engine::CPanel::FindChildGameObject(const std::wstring& _wstrGameObjectName)
{
	for (auto& iter : m_vecChildGameObjects)
	{
		if (_wstrGameObjectName == iter->GetGameObjectName())
		{
			return iter;
		}
	}

	return nullptr;
}

void Engine::CPanel::LoadJson(shared_ptr<CPanel> _spPanel, rapidjson::Document& _doc, int32& iPanelIndex)
{
	// ���⼭ �г��� ���ڸ� �Ű��ݴϴ�.
	string strPanelValue = "Panels" + to_string(iPanelIndex);

	// �� ���Ͽ��� �����͸� �о�ɴϴ�.
	rapidjson::Value& iComponentsSize = _doc[strPanelValue.c_str()]["Components Size"];

	// ���ӿ�����Ʈ�� �̸��� ����Ÿ���� �����մϴ�.
	rapidjson::Value& wstrPanelName = _doc[strPanelValue.c_str()]["Panel Name"];
	rapidjson::Value& eProjectionType = _doc[strPanelValue.c_str()]["Panel Projection Type"];

	_spPanel->SetGameObjectName(CStringUtility::ToWideString(wstrPanelName.GetString()));
	_spPanel->SetUIProjectionType((EProjectionType)eProjectionType.GetInt());

	rapidjson::Value& iPanelDepth = _doc[strPanelValue.c_str()]["Panel Depth"];
	_spPanel->SetDepth(iPanelDepth.GetInt());

	//if (EProjectionType::PERSPECTIVE == _spPanel->GetUIProjectionType())
	//{
	//	rapidjson::Value& wstrPivotComponentName = _doc[strPanelValue.c_str()]["Panel Pivot Component Name"];
	//}

	// ������ ������Ʈ�� ����ŭ for���� �����ݴϴ�.
	for (int32 i = 0; i < iComponentsSize.GetInt(); i++)
	{
		string strValue = "Components" + to_string(i);

		rapidjson::Value& ComponentIndex = _doc[strPanelValue.c_str()][strValue.c_str()];

		// ������Ʈ ����ü�� �����ؼ� ���� �������� ä���ݴϴ�. (RectComponent)
		CRectComponent::FRectComponentDesc fRectComponentDesc;

		fRectComponentDesc.m_wpOwnerGameObject = _spPanel;

		fRectComponentDesc.m_wstrComponentName = CStringUtility::ToWideString(ComponentIndex[0]["Component Name"].GetString());
		fRectComponentDesc.m_eComponentType = EComponentType::RECT;
		fRectComponentDesc.m_eRenderType = (ERenderType)ComponentIndex[0]["Render Type"].GetInt();
		fRectComponentDesc.m_eUIType = (EUIType)ComponentIndex[0]["UI Type"].GetInt();
		fRectComponentDesc.m_wstrShaderFileName = CStringUtility::ToWideString(ComponentIndex[0]["ShaderFileName"].GetString());
		fRectComponentDesc.m_wstrMaskTextureFileName = CStringUtility::ToWideString(ComponentIndex[0]["Mask Texture Name"].GetString());
		fRectComponentDesc.m_iPassIndex = ComponentIndex[0]["PassIndex"].GetInt();
		fRectComponentDesc.m_iDepth = ComponentIndex[0]["Depth"].GetInt();
		fRectComponentDesc.m_fAlpha = ComponentIndex[0]["Alpha"].GetFloat();
		fRectComponentDesc.m_fRectAngle = ComponentIndex[0]["Angle"].GetFloat();
		fRectComponentDesc.m_bMask = ComponentIndex[0]["IsMask"].GetBool();
		fRectComponentDesc.m_bAlpha = ComponentIndex[0]["IsAlpha"].GetBool();
		fRectComponentDesc.m_bColor = ComponentIndex[0]["IsColor"].GetBool();

		fRectComponentDesc.m_vColor.x = ComponentIndex[0]["Color"][0].GetFloat();
		fRectComponentDesc.m_vColor.y = ComponentIndex[0]["Color"][1].GetFloat();
		fRectComponentDesc.m_vColor.z = ComponentIndex[0]["Color"][2].GetFloat();
		fRectComponentDesc.m_vColor.w = ComponentIndex[0]["Color"][3].GetFloat();

		fRectComponentDesc.m_bInteraction = ComponentIndex[0]["IsInteraction"].GetBool();
		// ����ü�� ���������� �������� ������ ������Ʈ���� �ʱ�ȭ���ݴϴ�.

		// ��ü���� ������Ʈ ����ü�� ĳ��Ʈ�Ͽ� CreateFromJsom���� �Ѱ��ݴϴ�.
		// �� �� �ִϸ��̼� �̹��� ������Ʈ�� ���ܷ� ó���մϴ�.
		if (EUIType::STATIC_IMAGE == (EUIType)ComponentIndex[0]["UI Type"].GetInt())
		{
			shared_ptr<CImageComponent> spImageComponent = CImageComponent::CreateFromJson(make_shared<CImageComponent::FImageComponentDesc>(fRectComponentDesc), _doc, iPanelIndex, i);
			_spPanel->AddComponent(fRectComponentDesc.m_wstrComponentName, spImageComponent);
			if (/*EProjectionType::PERSPECTIVE == _spPanel->GetUIProjectionType() && */CStringUtility::ToWideString(_doc[strPanelValue.c_str()]["Panel Pivot Component Name"].GetString()) == spImageComponent->GetComponentName())
			{
				_spPanel->SetPivotComponent(spImageComponent);
			}
		}
		else if (EUIType::BUTTON == (EUIType)ComponentIndex[0]["UI Type"].GetInt())
		{
			shared_ptr<CButtonComponent> spButtonComponent = CButtonComponent::CreateFromJson(make_shared<CButtonComponent::FButtonComponentDesc>(fRectComponentDesc), _doc, iPanelIndex, i);
			_spPanel->AddComponent(fRectComponentDesc.m_wstrComponentName, spButtonComponent);
			if (CStringUtility::ToWideString(_doc[strPanelValue.c_str()]["Panel Pivot Component Name"].GetString()) == spButtonComponent->GetComponentName())
			{
				_spPanel->SetPivotComponent(spButtonComponent);
			}
		}
		else if (EUIType::SLIDER == (EUIType)ComponentIndex[0]["UI Type"].GetInt())
		{
			shared_ptr<CSliderComponent> spSliderComponent = CSliderComponent::CreateFromJson(make_shared<CSliderComponent::FSliderComponentDesc>(fRectComponentDesc), _doc, iPanelIndex, i);

			std::wstring wstrBackGroundName = CStringUtility::ToWideString(ComponentIndex[0]["BackGround Component Name"].GetString());
			shared_ptr<CImageComponent> spIamgeComponent = _spPanel->FindComponent<CImageComponent>(wstrBackGroundName);
			// �����̴� ������Ʈ�� �̹��� ������Ʈ�� �������ݴϴ�.
			spSliderComponent->SetBackGroundSliderComponent(spIamgeComponent);

			_spPanel->AddComponent(fRectComponentDesc.m_wstrComponentName, spSliderComponent);
			if (/*EProjectionType::PERSPECTIVE == _spPanel->GetUIProjectionType() && */CStringUtility::ToWideString(_doc[strPanelValue.c_str()]["Panel Pivot Component Name"].GetString()) == spSliderComponent->GetComponentName())
			{
				_spPanel->SetPivotComponent(spSliderComponent);
			}
		}
		else if (EUIType::CIRCULAR_PROGRESSBAR == (EUIType)ComponentIndex[0]["UI Type"].GetInt() || EUIType::LINEAR_HORIZONTAL_PROGRESSBAR == (EUIType)ComponentIndex[0]["UI Type"].GetInt() || EUIType::LINEAR_VERTICAL_PROGRESSBAR == (EUIType)ComponentIndex[0]["UI Type"].GetInt())
		{
			shared_ptr<CProgressBarComponent> spProgressComponent = CProgressBarComponent::CreateFromJson(make_shared<CProgressBarComponent::FProgressBarComponentDesc>(fRectComponentDesc), _doc, iPanelIndex, i);
			_spPanel->AddComponent(fRectComponentDesc.m_wstrComponentName, spProgressComponent);
			if (/*EProjectionType::PERSPECTIVE == _spPanel->GetUIProjectionType() && */CStringUtility::ToWideString(_doc[strPanelValue.c_str()]["Panel Pivot Component Name"].GetString()) == spProgressComponent->GetComponentName())
			{
				_spPanel->SetPivotComponent(spProgressComponent);
			}
		}
		else if (EUIType::DYNAMIC_IMAGE == (EUIType)ComponentIndex[0]["UI Type"].GetInt())
		{
			// Ű�������� ����� �Ѱ��ݴϴ�.
			int32 iKeyFrameIndex = ComponentIndex[0]["KeyFrame Size"].GetUint();

			shared_ptr<CAnimationImageComponent> spAniImageComponent = CAnimationImageComponent::CreateFromJson(make_shared<CAnimationImageComponent::FAnimationImageComponentDesc>(fRectComponentDesc), _doc, iPanelIndex, i, iKeyFrameIndex);
			_spPanel->AddComponent(fRectComponentDesc.m_wstrComponentName, spAniImageComponent);

			if (/*EProjectionType::PERSPECTIVE == _spPanel->GetUIProjectionType() && */CStringUtility::ToWideString(_doc[strPanelValue.c_str()]["Panel Pivot Component Name"].GetString()) == spAniImageComponent->GetComponentName())
			{
				_spPanel->SetPivotComponent(spAniImageComponent);
			}
		}
		else if (EUIType::TEXT == (EUIType)ComponentIndex[0]["UI Type"].GetInt())
		{
			shared_ptr<CTextComponent> spTextComponent = CTextComponent::CreateFromJson(make_shared<CTextComponent::FTextComponentDesc>(fRectComponentDesc), _doc, iPanelIndex, i);
			_spPanel->AddComponent(fRectComponentDesc.m_wstrComponentName, spTextComponent);

			if (/*EProjectionType::PERSPECTIVE == _spPanel->GetUIProjectionType() && */CStringUtility::ToWideString(_doc[strPanelValue.c_str()]["Panel Pivot Component Name"].GetString()) == spTextComponent->GetComponentName())
			{
				_spPanel->SetPivotComponent(spTextComponent);
			}
		}
		else if (EUIType::FRAME_IMAGE == (EUIType)ComponentIndex[0]["UI Type"].GetInt())
		{
			int iKey = 0;
			shared_ptr<CAnimationImageComponent> spAniImageComponent = CAnimationImageComponent::CreateFromJson(make_shared<CAnimationImageComponent::FAnimationImageComponentDesc>(fRectComponentDesc), _doc, iPanelIndex, i, iKey);
			_spPanel->AddComponent(fRectComponentDesc.m_wstrComponentName, spAniImageComponent);

			if (/*EProjectionType::PERSPECTIVE == _spPanel->GetUIProjectionType() && */CStringUtility::ToWideString(_doc[strPanelValue.c_str()]["Panel Pivot Component Name"].GetString()) == spAniImageComponent->GetComponentName())
			{
				_spPanel->SetPivotComponent(spAniImageComponent);
			}
		}
		else if (EUIType::LINEAR_INTERPOLATION_PROGRESSBAR == (EUIType)ComponentIndex[0]["UI Type"].GetInt() || EUIType::CIRCLE_INTERPOLATION_PROGRESSBAR == (EUIType)ComponentIndex[0]["UI Type"].GetInt())
		{
			shared_ptr<CProgressBarComponent> spProgressComponent = CProgressBarComponent::CreateFromJson(make_shared<CProgressBarComponent::FProgressBarComponentDesc>(fRectComponentDesc), _doc, iPanelIndex, i);
			_spPanel->AddComponent(fRectComponentDesc.m_wstrComponentName, spProgressComponent);
			if (/*EProjectionType::PERSPECTIVE == _spPanel->GetUIProjectionType() && */CStringUtility::ToWideString(_doc[strPanelValue.c_str()]["Panel Pivot Component Name"].GetString()) == spProgressComponent->GetComponentName())
			{
				_spPanel->SetPivotComponent(spProgressComponent);
			}
		}
	}

	// Transform Component
	shared_ptr<CTransformComponent::FTransformComponentDesc> spTransformComponentDesc = make_shared<CTransformComponent::FTransformComponentDesc>();
	spTransformComponentDesc->m_wpOwnerGameObject = shared_from_this();
	spTransformComponentDesc->m_eComponentType = EComponentType::TRANSFORM;
	spTransformComponentDesc->m_wstrComponentName = L"TransformComponent";
	spTransformComponentDesc->m_eRenderType = ERenderType::NONE;

	shared_ptr<CTransformComponent> spTransformComponent = CTransformComponent::Create(spTransformComponentDesc);
	AddComponent(L"TransformComponent", spTransformComponent);
	m_wpTransformComponent = spTransformComponent;
}

const std::shared_ptr<CRectComponent> Engine::CPanel::GetPivotComponent()
{
	return m_spPivotComponent;
	// ���� �θ� �г��� ����Դϴ�.
	//if (nullptr == m_wpParentGameObject.lock())
	//{
	//	return m_spPivotComponent;
	//}
	//// �ڽ� �г��� ��� �θ� �г��� pivotcomponent�� �޾ƿɴϴ�.
	//else
	//{
	//	return dynamic_pointer_cast<CPanel>(m_wpParentGameObject.lock())->GetPivotComponent();
	//}
}

void Engine::CPanel::SetPivotWorldPoint(Vector3 _vPivotWorldPoint)
{
	m_spPivotComponent->SetRectPosition(_vPivotWorldPoint);
}

const Vector3 Engine::CPanel::GetPivotWorldPoint()
{
	return m_spPivotComponent->GetRectPosition();
}

void Engine::CPanel::SetPivotCenterPoint(Vector2 _vPivotCenterPoint)
{
	m_spPivotComponent->SetCenterPosition(_vPivotCenterPoint);
}

const Vector2 Engine::CPanel::GetPivotCenterPoint()
{
	return m_spPivotComponent->GetCenterPosition();
}
