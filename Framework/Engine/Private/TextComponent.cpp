#include "pch.h"
#include "TextComponent.h"

#include "FontHandler.h"
#include "Font.h"

#include "StringUtility.h"
#include "UI.h"
#include "Panel.h"

std::shared_ptr<CTextComponent> Engine::CTextComponent::Create(const std::shared_ptr<FTextComponentDesc>& _spTextComponentDesc)
{
	const std::shared_ptr<CTextComponent> spTextComponent = make_shared<CTextComponent>();
	spTextComponent->InitializeComponent(_spTextComponentDesc);
	return spTextComponent;
}

std::shared_ptr<CTextComponent> Engine::CTextComponent::CreateFromJson(const std::shared_ptr<FTextComponentDesc>& _spTextComponentDesc, rapidjson::Document& _doc, int32& iPanelIndex, int32& iComponentIndex)
{
	const std::shared_ptr<CTextComponent> spTextComponent = make_shared<CTextComponent>();

	// 값들을 멤버 변수에 저장해줍니다.
	string strPanelValue = "Panels" + to_string(iPanelIndex);

	string strValue = "Components" + to_string(iComponentIndex);
	rapidjson::Value& ComponentIndex = _doc[strPanelValue.c_str()][strValue.c_str()];

	spTextComponent->SetCenterPosition(Vector2(ComponentIndex[0]["Center"][0].GetFloat(), ComponentIndex[0]["Center"][1].GetFloat()));
	spTextComponent->SetRectPosition(Vector3(ComponentIndex[0]["World Position"][0].GetFloat(), ComponentIndex[0]["World Position"][1].GetFloat(), ComponentIndex[0]["World Position"][2].GetFloat()));
	spTextComponent->SetRectScale(Vector3(ComponentIndex[0]["Scale"][0].GetFloat(), ComponentIndex[0]["Scale"][1].GetFloat(), ComponentIndex[0]["Scale"][2].GetFloat()));
	spTextComponent->SetRatio(ComponentIndex[0]["Ratio"].GetFloat());

	spTextComponent->SetOwnerGameObject((_spTextComponentDesc->m_wpOwnerGameObject).lock());
	if (EProjectionType::PERSPECTIVE == dynamic_pointer_cast<CUI>(spTextComponent->GetOwnerGameObject())->GetUIProjectionType())
	{
		spTextComponent->SetScreenRelativeCenter(Vector2(ComponentIndex[0]["RelativeCenter"][0].GetFloat(), ComponentIndex[0]["RelativeCenter"][1].GetFloat()));
	}

	if (EProjectionType::ORTHOGONAL == dynamic_pointer_cast<CUI>(spTextComponent->GetOwnerGameObject())->GetUIProjectionType())
	{
		spTextComponent->IsRelativeCenter(ComponentIndex[0]["IsRelative"].GetBool());
		if (true == spTextComponent->IsRelativeCenter())
		{
			spTextComponent->SetScreenRelativeCenter(Vector2(ComponentIndex[0]["RelativeCenter"][0].GetFloat(), ComponentIndex[0]["RelativeCenter"][1].GetFloat()));
		}
	}

	_spTextComponentDesc->m_wstrFontFileName = CStringUtility::ToWideString(ComponentIndex[0]["Font File Name"].GetString());
	_spTextComponentDesc->m_wstrText = CStringUtility::ToWideString(ComponentIndex[0]["Text"].GetString());
	_spTextComponentDesc->m_vOrigin.x = ComponentIndex[0]["Origin"][0].GetFloat();
	_spTextComponentDesc->m_vOrigin.y = ComponentIndex[0]["Origin"][1].GetFloat();

	 _spTextComponentDesc->m_iOption = ComponentIndex[0]["Option"].GetInt();

	 _spTextComponentDesc->m_eTextType = static_cast<ETextType>(ComponentIndex[0]["Text Type"].GetInt());

	spTextComponent->InitializeComponent(_spTextComponentDesc);
	return spTextComponent;
}

HRESULT Engine::CTextComponent::InitializeComponent(const std::shared_ptr<FComponentDesc>& _spComponentDesc)
{
	// rect component desc
	if (FAILED(CRectComponent::InitializeComponent(_spComponentDesc))) { return E_FAIL; }

	// text component desc
	shared_ptr<FTextComponentDesc> spTextComponentDesc = static_pointer_cast<FTextComponentDesc>(_spComponentDesc);

	m_wstrFontFileName = spTextComponentDesc->m_wstrFontFileName;
	m_vOrigin = spTextComponentDesc->m_vOrigin;
	m_wstrText = spTextComponentDesc->m_wstrText;
	m_iOption = 0x0;
	m_eTextType = spTextComponentDesc->m_eTextType;

	// etc
	// 복사해서 새로운 객체로 들고 있어야합니다.
	// 아니면 모두 동기화됩니다.
	shared_ptr<CFont> spFont = CFontHandler::GetInstance()->FindFont(m_wstrFontFileName);
	m_spFont = make_shared<CFont>(*spFont);
	m_spFont->SetTextComponent(static_pointer_cast<CTextComponent>(shared_from_this()));

	return S_OK;
}

HRESULT Engine::CTextComponent::BeginPlay()
{
	if (FAILED(CRectComponent::BeginPlay())) { return E_FAIL; }

	return S_OK;
}

int32 Engine::CTextComponent::PreTickComponent(float _fDeltaSeconds)
{
	switch (CRectComponent::PreTickComponent(_fDeltaSeconds))
	{
	case -1:	return -1; 
	default:	break;
	}

	return 0;
}

int32 Engine::CTextComponent::TickComponent(float _fDeltaSeconds)
{
	switch (CRectComponent::TickComponent(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	//if (EProjectionType::PERSPECTIVE == dynamic_pointer_cast<CUI>(m_wpOwnerGameObject.lock())->GetUIProjectionType())
	//{
	//	m_vCenter = m_vWorldToScreenPosition;
	//}
	//else if (EProjectionType::ORTHOGONAL == dynamic_pointer_cast<CUI>(m_wpOwnerGameObject.lock())->GetUIProjectionType())
	//{
	//	// 직교일 경우

	//}

	//SetFontDesc();

	return 0;
}

int32 Engine::CTextComponent::PostTickComponent(float _fDeltaSeconds)
{
	switch (CRectComponent::PostTickComponent(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	if (EProjectionType::PERSPECTIVE == dynamic_pointer_cast<CUI>(m_wpOwnerGameObject.lock())->GetUIProjectionType())
	{
		m_vCenter = m_vWorldToScreenPosition;
	}
	else if (EProjectionType::ORTHOGONAL == dynamic_pointer_cast<CUI>(m_wpOwnerGameObject.lock())->GetUIProjectionType())
	{
		// 직교일 경우

	}

	SetFontDesc();

	return 0;
}

HRESULT Engine::CTextComponent::EndPlay()
{
	if (FAILED(CRectComponent::EndPlay())) { return E_FAIL; }

	return S_OK;
}

void Engine::CTextComponent::Render(int32 _iShaderPath)
{
	m_spFont->Render();
}

void Engine::CTextComponent::Release()
{
	m_wstrText.clear();
	m_spFont = nullptr;

	CRectComponent::Release();
}

HRESULT Engine::CTextComponent::BindVariable()
{
	return CRectComponent::BindVariable();
}

void Engine::CTextComponent::SetFontDesc(FFontDesc& fFontDesc)
{
	m_spFont->SetText(fFontDesc.m_wstrText);
	m_spFont->SetScreenPosition(fFontDesc.m_vScreenPosition);
	m_spFont->SetColor(fFontDesc.m_vColor);
	m_spFont->SetRadian(fFontDesc.m_fRadian);
	m_spFont->SetOrigin(fFontDesc.m_vOrigin);
	m_spFont->SetScale(fFontDesc.m_fScale);
	m_spFont->SetOption(fFontDesc.m_iOption);
}

void Engine::CTextComponent::SetFontDesc()
{
	m_spFont->SetText(m_wstrText);
	m_spFont->SetScreenPosition(m_vCenter);
	m_spFont->SetColor(m_vColor);
	m_spFont->SetRadian(XMConvertToRadians(m_fRectAngle));
	m_spFont->SetOrigin(m_vOrigin);
	// 첫번째 스케일 값을 넘겨줍니다.
	m_spFont->SetScale(GetRectScale().x);
	m_spFont->SetOption(0x0);
}

FFontDesc Engine::CTextComponent::GetFontDesc()
{
	FFontDesc fFontDesc;

	fFontDesc.m_wstrText = m_wstrText;
	fFontDesc.m_vScreenPosition = m_vCenter;
	fFontDesc.m_vColor = m_vColor;

	fFontDesc.m_fRadian = m_fRectAngle;
	fFontDesc.m_vOrigin = m_vOrigin;
	fFontDesc.m_fScale = GetRectScale().x;
	fFontDesc.m_iOption = 0x0;

	return fFontDesc;
}

void Engine::CTextComponent::SetScreenPosition(Vector2 _vScreenPosition)
{
	m_spFont->SetScreenPosition(_vScreenPosition);
}

const Vector2 Engine::CTextComponent::GetScreenPosition() const
{
	return m_spFont->GetScreenPosition();
}
