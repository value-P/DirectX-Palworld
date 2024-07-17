#include "pch.h"
#include "SliderComponent.h"

#include "PipelineManager.h"
#include "EffectShader.h"
#include "TextureHandler.h"
#include "ShaderHandler.h"

#include "EngineModule.h"
#include "InputManager.h"

#include "StringUtility.h"

std::shared_ptr<CSliderComponent> Engine::CSliderComponent::Create(const std::shared_ptr<FSliderComponentDesc>& _spSliderComponentDesc)
{
	std::shared_ptr<CSliderComponent> spSliderComponent = make_shared<CSliderComponent>();
	ENSUREF(spSliderComponent->InitializeComponent(_spSliderComponentDesc), L"Failed to initialize slider component.");
	return spSliderComponent;
}

std::shared_ptr<CSliderComponent> Engine::CSliderComponent::CreateFromJson(const std::shared_ptr<FSliderComponentDesc>& _spSliderComponentDesc, rapidjson::Document& _doc, int32& iPanelIndex, int32& iComponentIndex)
{
	std::shared_ptr<CSliderComponent> spSliderComponent = make_shared<CSliderComponent>();

	string strPanelValue = "Panels" + to_string(iPanelIndex);

	string strValue = "Components" + to_string(iComponentIndex);

	rapidjson::Value& ComponentIndex = _doc[strPanelValue.c_str()][strValue.c_str()];

	spSliderComponent->SetCenterPosition(Vector2(ComponentIndex[0]["Center"][0].GetFloat(), ComponentIndex[0]["Center"][1].GetFloat()));
	spSliderComponent->SetRectPosition(Vector3(ComponentIndex[0]["World Position"][0].GetFloat(), ComponentIndex[0]["World Position"][1].GetFloat(), ComponentIndex[0]["World Position"][2].GetFloat()));
	spSliderComponent->SetRectScale(Vector3(ComponentIndex[0]["Scale"][0].GetFloat(), ComponentIndex[0]["Scale"][1].GetFloat(), ComponentIndex[0]["Scale"][2].GetFloat()));
	spSliderComponent->SetRatio(ComponentIndex[0]["Ratio"].GetFloat());

	_spSliderComponentDesc->m_wstrTextureSliderButtonFileName = CStringUtility::ToWideString(ComponentIndex[0]["Slider Texture Name"].GetString());

	ENSUREF(spSliderComponent->InitializeComponent(_spSliderComponentDesc), L"Failed to initialize slider component.");
	return spSliderComponent;
}

HRESULT Engine::CSliderComponent::InitializeComponent(const std::shared_ptr<FComponentDesc>& _spComponentDesc)
{
	// rect component desc
	if (FAILED(CRectComponent::InitializeComponent(_spComponentDesc))) { return E_FAIL; }

	// slider component
	shared_ptr<FSliderComponentDesc> spSliderComponentDesc = static_pointer_cast<FSliderComponentDesc>(_spComponentDesc);
	m_wstrTextureSliderButtonFileName = spSliderComponentDesc->m_wstrTextureSliderButtonFileName;
	m_wpBackGroundSliderComponent = spSliderComponentDesc->m_wpBackGroundSliderComponent;

	// SRV : 1번 슬롯에 Base 텍스처를 넣음
	m_vecSRVs.push_back(CTextureHandler::GetInstance()->FindShaderResourceView(m_wstrTextureSliderButtonFileName, ETextureType::UI).Get());

	return S_OK;
}

HRESULT Engine::CSliderComponent::BeginPlay()
{
	if (FAILED(CRectComponent::BeginPlay())) { return E_FAIL; }

	return S_OK;
}

int32 Engine::CSliderComponent::PreTickComponent(float _fDeltaSeconds)
{
	// BackGroundSlider를 기준으로 Min과 Max를 설정해줍니다.
	// 연결 되어 있지 않다면 return하게 합니다.
	if (nullptr != m_wpBackGroundSliderComponent.lock())
	{
		m_fMin = m_wpBackGroundSliderComponent.lock()->GetCenterPosition().x - (m_wpBackGroundSliderComponent.lock()->GetRectScale().x * 0.5f);
		m_fMax = m_wpBackGroundSliderComponent.lock()->GetCenterPosition().x + (m_wpBackGroundSliderComponent.lock()->GetRectScale().x * 0.5f);

		// 만약 센터의 x점이 설정한 Min or Max값을 넘어갔거나 같다면 값을 제한해줍니다.
		if (m_fMin >= m_vCenter.x)
		{
			m_vCenter.x = m_fMin;
		}
		else if (m_fMax <= m_vCenter.x)
		{
			m_vCenter.x = m_fMax;
		}

		// 센터의 y점은 BackGroundSlider에 종속적이게 만듭니다.
		m_vCenter.y = m_wpBackGroundSliderComponent.lock()->GetCenterPosition().y;

		Normalize();

		m_fNormalize;

		m_wpBackGroundSliderComponent.lock()->SetRatio(m_fNormalize);
	}

	switch (CRectComponent::PreTickComponent(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return 0;
}

int32 Engine::CSliderComponent::TickComponent(float _fDeltaSeconds)
{
	switch (CRectComponent::TickComponent(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return 0;
}

int32 Engine::CSliderComponent::PostTickComponent(float _fDeltaSeconds)
{
	switch (CRectComponent::PostTickComponent(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return 0;
}

HRESULT Engine::CSliderComponent::EndPlay()
{
	if (FAILED(CRectComponent::EndPlay())) { return E_FAIL; }

	return S_OK;
}

void Engine::CSliderComponent::Render(int32 _iShaderPath)
{
	if (m_wstrShaderFileName.empty()) { return; }

	CRectComponent::Render(_iShaderPath);
}

void Engine::CSliderComponent::Release()
{
	CRectComponent::Release();
}

HRESULT Engine::CSliderComponent::BindVariable()
{
	return 	CRectComponent::BindVariable();
}

bool Engine::CSliderComponent::IsDragged()
{
	// 마우스를 뗄 때까지 계속 누른 상태를 유지하면서 움직일 수 있게 합니다.
	if (m_bFocusedControl && CEngineModule::GetInstance()->GetInputManager()->IsMousePressed(EMouseActionType::LEFT_BUTTON))
	{
		AddMouseMove();
		return true;
	}

	return false;
}

void Engine::CSliderComponent::Normalize()
{
	m_fNormalize = (m_vCenter.x - m_fMin) / (m_fMax - m_fMin);
}

std::wstring Engine::CSliderComponent::GetBackGroundComponentName() const
{
	return m_wpBackGroundSliderComponent.lock()->GetComponentName();
}



