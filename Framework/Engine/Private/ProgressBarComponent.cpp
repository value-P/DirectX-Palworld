#include "pch.h"
#include "ProgressBarComponent.h"

#include "EffectShader.h"
#include "TextureHandler.h"

#include "StringUtility.h"
#include "UI.h"

std::shared_ptr<CProgressBarComponent> Engine::CProgressBarComponent::Create(const std::shared_ptr<FProgressBarComponentDesc>& _spProgressComponentDesc)
{
	const std::shared_ptr<CProgressBarComponent> spProgressComponent = make_shared<CProgressBarComponent>();
	ENSUREF(spProgressComponent->InitializeComponent(_spProgressComponentDesc), L"Failed to initialize progress bar component");
	return spProgressComponent;
}

std::shared_ptr<CProgressBarComponent> Engine::CProgressBarComponent::CreateFromJson(const std::shared_ptr<FProgressBarComponentDesc>& _spProgressComponentDesc, rapidjson::Document& _doc, int32& iPanelIndex, int32& iComponentIndex)
{
	const std::shared_ptr<CProgressBarComponent> spProgressComponent = make_shared<CProgressBarComponent>();

	string strPanelValue = "Panels" + to_string(iPanelIndex);
	string strValue = "Components" + to_string(iComponentIndex);

	rapidjson::Value& ComponentIndex = _doc[strPanelValue.c_str()][strValue.c_str()];

	spProgressComponent->SetUIType(static_cast<EUIType>(_doc[strPanelValue.c_str()][strValue.c_str()][0]["UI Type"].GetInt()));

	spProgressComponent->SetCenterPosition(Vector2(ComponentIndex[0]["Center"][0].GetFloat(), ComponentIndex[0]["Center"][1].GetFloat()));
	spProgressComponent->SetRectPosition(Vector3(ComponentIndex[0]["World Position"][0].GetFloat(), ComponentIndex[0]["World Position"][1].GetFloat(), ComponentIndex[0]["World Position"][2].GetFloat()));
	spProgressComponent->SetRectScale(Vector3(ComponentIndex[0]["Scale"][0].GetFloat(), ComponentIndex[0]["Scale"][1].GetFloat(), ComponentIndex[0]["Scale"][2].GetFloat()));
	spProgressComponent->SetRatio(ComponentIndex[0]["Ratio"].GetFloat());

	spProgressComponent->SetOwnerGameObject((_spProgressComponentDesc->m_wpOwnerGameObject).lock());
	if (EProjectionType::PERSPECTIVE == dynamic_pointer_cast<CUI>(spProgressComponent->GetOwnerGameObject())->GetUIProjectionType())
	{
		spProgressComponent->SetScreenRelativeCenter(Vector2(ComponentIndex[0]["RelativeCenter"][0].GetFloat(), ComponentIndex[0]["RelativeCenter"][1].GetFloat()));
	}
	if (EProjectionType::ORTHOGONAL == dynamic_pointer_cast<CUI>(spProgressComponent->GetOwnerGameObject())->GetUIProjectionType())
	{
		spProgressComponent->IsRelativeCenter(ComponentIndex[0]["IsRelative"].GetBool());
		if (true == spProgressComponent->IsRelativeCenter())
		{
			spProgressComponent->SetScreenRelativeCenter(Vector2(ComponentIndex[0]["RelativeCenter"][0].GetFloat(), ComponentIndex[0]["RelativeCenter"][1].GetFloat()));
		}
	}
	_spProgressComponentDesc->m_wstrTextureFileName = CStringUtility::ToWideString(ComponentIndex[0]["Progress Texture Name"].GetString());

	if (EUIType::LINEAR_INTERPOLATION_PROGRESSBAR == spProgressComponent->GetUIType() || EUIType::CIRCLE_INTERPOLATION_PROGRESSBAR == spProgressComponent->GetUIType())
	{
		spProgressComponent->SetOtherColor(Vector4(ComponentIndex[0]["Other Color"][0].GetFloat(), ComponentIndex[0]["Other Color"][1].GetFloat(), ComponentIndex[0]["Other Color"][2].GetFloat(),
			ComponentIndex[0]["Other Color"][3].GetFloat()));
		spProgressComponent->SetOtherRatio(ComponentIndex[0]["Other Ratio"].GetFloat());
	}

	spProgressComponent->InitializeComponent(_spProgressComponentDesc);
	return spProgressComponent;
}

HRESULT Engine::CProgressBarComponent::InitializeComponent(const std::shared_ptr<FComponentDesc>& _spComponentDesc)
{
	// rect component desc
	if (FAILED(CRectComponent::InitializeComponent(_spComponentDesc))) { return E_FAIL; }

	// progressbar component desc
	shared_ptr<FProgressBarComponentDesc> spProgressBarComponentDesc = static_pointer_cast<FProgressBarComponentDesc>(_spComponentDesc);
	m_wstrTextureFileName = spProgressBarComponentDesc->m_wstrTextureFileName;

	// SRV : 1번 슬롯에 Base텍스처를 넣음
	m_vecSRVs.push_back(CTextureHandler::GetInstance()->FindShaderResourceView(m_wstrTextureFileName, ETextureType::UI).Get());

	// Bind Variable 변수를 가져옴
	m_cpVariable_ProgressBar = m_wpEffectShader.lock()->GetRawVariableByName("g_tUiInfo_ProgressBar");

	return S_OK;
}

HRESULT Engine::CProgressBarComponent::BeginPlay()
{
	if (FAILED(CRectComponent::BeginPlay())) { return E_FAIL; }

	return S_OK;
}

int32 Engine::CProgressBarComponent::PreTickComponent(float _fDeltaSeconds)
{
	switch (CRectComponent::PreTickComponent(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return 0;
}

int32 Engine::CProgressBarComponent::TickComponent(float _fDeltaSeconds)
{
	switch (CRectComponent::TickComponent(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return 0;
}

int32 Engine::CProgressBarComponent::PostTickComponent(float _fDeltaSeconds)
{
	switch (CRectComponent::PostTickComponent(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return 0;
}

HRESULT Engine::CProgressBarComponent::EndPlay()
{
	if (FAILED(CRectComponent::EndPlay())) { return E_FAIL; }

	return S_OK;
}

void Engine::CProgressBarComponent::Render(int32 _iShaderPath)
{
	CRectComponent::Render(_iShaderPath);
}

void Engine::CProgressBarComponent::Release()
{
	m_cpVariable_ProgressBar = nullptr;
	CRectComponent::Release();
}

HRESULT Engine::CProgressBarComponent::BindVariable()
{
	ENSURE(m_cpVariable_ProgressBar->SetRawValue(&m_tUiInfoProgressBar, 0, sizeof(m_tUiInfoProgressBar)));
	return CRectComponent::BindVariable();
}
 