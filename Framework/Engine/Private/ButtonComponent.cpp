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

	// SRV : �� �ؽ�ó�� �غ��� �ΰ�, SRV ������ ũ�⸦ �ø��� ���� nullptr ����
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
	// Button�� ���¿� ���� �ؽ��ĸ� Bind ���ݴϴ�.
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
		// ���콺 ��ġ�� Ȯ���մϴ�.
		if (IsInMouse())
		{
			// ��ư �̺�Ʈ�� �߻������ݴϴ�.
			if (m_fnButtonClick)
			{
				// �Լ��� ���� �Ǿ����� �� ȣ���մϴ�.
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
