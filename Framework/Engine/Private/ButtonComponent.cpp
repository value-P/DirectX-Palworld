#include "pch.h"
#include "ButtonComponent.h"

#include "EffectShader.h"
#include "TextureHandler.h"

#include "EngineModule.h"
#include "InputManager.h"
#include "SoundManager.h"

#include "StringUtility.h"

std::shared_ptr<CButtonComponent> Engine::CButtonComponent::Create(const std::shared_ptr<FButtonComponentDesc>& _spButtonComponentDesc)
{
	const std::shared_ptr<CButtonComponent> spButtonComponent = make_shared<CButtonComponent>();
	spButtonComponent->InitializeComponent(_spButtonComponentDesc);
	return spButtonComponent;
}

std::shared_ptr<CButtonComponent> Engine::CButtonComponent::CreateFromJson(const std::shared_ptr<FButtonComponentDesc>& _spButtonComponentDesc, rapidjson::Document& _doc, int32& iPanelIndex, int32& iComponentIndex)
{
	const std::shared_ptr<CButtonComponent> spButtonComponent = make_shared<CButtonComponent>();

	string strPanelValue = "Panels" + to_string(iPanelIndex);

	string strValue = "Components" + to_string(iComponentIndex);

	rapidjson::Value& ComponentIndex = _doc[strPanelValue.c_str()][strValue.c_str()];

	spButtonComponent->SetCenterPosition(Vector2(ComponentIndex[0]["Center"][0].GetFloat(), ComponentIndex[0]["Center"][1].GetFloat()));
	spButtonComponent->SetRectPosition(Vector3(ComponentIndex[0]["World Position"][0].GetFloat(), ComponentIndex[0]["World Position"][1].GetFloat(), ComponentIndex[0]["World Position"][2].GetFloat()));
	spButtonComponent->SetRectScale(Vector3(ComponentIndex[0]["Scale"][0].GetFloat(), ComponentIndex[0]["Scale"][1].GetFloat(), ComponentIndex[0]["Scale"][2].GetFloat()));
	spButtonComponent->SetRatio(ComponentIndex[0]["Ratio"].GetFloat());

	_spButtonComponentDesc->m_wstrTextureFileNameButtonStatic = CStringUtility::ToWideString(ComponentIndex[0]["Static Texture Name"].GetString());
	_spButtonComponentDesc->m_wstrTextureFileNameButtonHover = CStringUtility::ToWideString(ComponentIndex[0]["Hover Texture Name"].GetString());
	_spButtonComponentDesc->m_wstrTextureFileNameButtonClick = CStringUtility::ToWideString(ComponentIndex[0]["Click Texture Name"].GetString());

	spButtonComponent->InitializeComponent(_spButtonComponentDesc);

	return spButtonComponent;
}

HRESULT Engine::CButtonComponent::InitializeComponent(const std::shared_ptr<FComponentDesc>& _spComponentDesc)
{
	// rect component desc
	if (FAILED(CRectComponent::InitializeComponent(_spComponentDesc))) { return E_FAIL; }

	// button component desc
	shared_ptr<FButtonComponentDesc> spButtonComponentDesc = static_pointer_cast<FButtonComponentDesc>(_spComponentDesc);
	m_wstrTextureFileNameButtonStatic = spButtonComponentDesc->m_wstrTextureFileNameButtonStatic;
	m_wstrTextureFileNameButtonHover = spButtonComponentDesc->m_wstrTextureFileNameButtonHover;
	m_wstrTextureFileNameButtonClick = spButtonComponentDesc->m_wstrTextureFileNameButtonClick;

	// SRV : 각 텍스처를 준비해 두고, SRV 벡터의 크기를 늘리기 위해 nullptr 삽입
	m_cpSRVButtonStatic = CTextureHandler::GetInstance()->FindShaderResourceView(m_wstrTextureFileNameButtonStatic, ETextureType::UI);
	m_cpSRVButtonHover = CTextureHandler::GetInstance()->FindShaderResourceView(m_wstrTextureFileNameButtonHover, ETextureType::UI);
	m_cpSRVButtonClick = CTextureHandler::GetInstance()->FindShaderResourceView(m_wstrTextureFileNameButtonClick, ETextureType::UI);
	m_vecSRVs.push_back(nullptr);

	return S_OK;
}

HRESULT Engine::CButtonComponent::BeginPlay()
{
	if (FAILED(CRectComponent::BeginPlay())) { return E_FAIL; }

	return S_OK;
}

int32 Engine::CButtonComponent::PreTickComponent(float _fDeltaSeconds)
{
	switch (CRectComponent::PreTickComponent(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return 0;
}

int32 Engine::CButtonComponent::TickComponent(float _fDeltaSeconds)
{
	switch (CRectComponent::TickComponent(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	//IsHoverd();
	//IsClicked();

	return 0;
}

int32 Engine::CButtonComponent::PostTickComponent(float _fDeltaSeconds)
{
	switch (CRectComponent::PostTickComponent(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return 0;
}

HRESULT Engine::CButtonComponent::EndPlay()
{
	if (FAILED(CRectComponent::EndPlay())) { return E_FAIL; }

	return S_OK;
}

void Engine::CButtonComponent::Render(int32 _iShaderPath)
{
	CRectComponent::Render(_iShaderPath);
	//m_fnButtonClick = nullptr;
}

void Engine::CButtonComponent::Release()
{
	m_cpSRVButtonStatic = nullptr;
	m_cpSRVButtonHover = nullptr;
	m_cpSRVButtonClick = nullptr;

	CRectComponent::Release();
}

HRESULT Engine::CButtonComponent::BindVariable()
{
	// Button의 상태에 따라 텍스쳐를 Bind 해줍니다.
	if (m_bClick)
	{
		m_vecSRVs[1]= m_cpSRVButtonClick.Get();
	}
	else if (m_bHover)
	{
		m_vecSRVs[1] = m_cpSRVButtonHover.Get();
	}
	else
	{
		m_vecSRVs[1] = m_cpSRVButtonStatic.Get();
	}

	return CRectComponent::BindVariable();
}

bool Engine::CButtonComponent::IsHoverd()
{
	if (IsInMouse())
	{
		m_bHover = true;
		return true;
	}

	m_bHover = false;
	return false;
}

bool Engine::CButtonComponent::IsClicked()
{
	if (m_bFocusedControl && m_bHover)
	{
		bool bSelect = CEngineModule::GetInstance()->GetInputManager()->IsMouseUp(EMouseActionType::LEFT_BUTTON);
		// 마우스 위치를 확인합니다.
		if (IsInMouse())
		{
			// 버튼 이벤트를 발생시켜줍니다.
			if (m_fnButtonClick)
			{
				// 함수가 설정 되어있을 시 호출합니다.
				m_fnButtonClick();
				m_fnButtonClick = nullptr;
			}

			return true;
		}
	}

    SharedNull(m_fnButtonClick);
    m_bClick = false;
    return false;
}
