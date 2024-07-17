#include "pch.h"
#include "ImageComponent.h"

#include "EffectShader.h"
#include "TextureHandler.h"

#include "StringUtility.h"
#include "UI.h"

std::shared_ptr<CImageComponent> Engine::CImageComponent::Create(const std::shared_ptr<FImageComponentDesc>& _spImageComponentDesc)
{
	const std::shared_ptr<CImageComponent> spImageComponent = make_shared<CImageComponent>();
	ENSUREF(spImageComponent->InitializeComponent(_spImageComponentDesc), L"Failed to initialize image component.");
	return spImageComponent;
}

std::shared_ptr<CImageComponent> Engine::CImageComponent::CreateFromJson(const std::shared_ptr<FImageComponentDesc>& _spImageComponentDesc, rapidjson::Document& _doc, int32& iPanelIndex, int32& iComponentIndex)
{
	const std::shared_ptr<CImageComponent> spImageComponent = make_shared<CImageComponent>();

	string strPanelValue = "Panels" + to_string(iPanelIndex);

	string strValue = "Components" + to_string(iComponentIndex);

	rapidjson::Value& ComponentIndex = _doc[strPanelValue.c_str()][strValue.c_str()];

	spImageComponent->SetCenterPosition(Vector2(ComponentIndex[0]["Center"][0].GetFloat(), ComponentIndex[0]["Center"][1].GetFloat()));
	spImageComponent->SetRectPosition(Vector3(ComponentIndex[0]["World Position"][0].GetFloat(), ComponentIndex[0]["World Position"][1].GetFloat(), ComponentIndex[0]["World Position"][2].GetFloat()));
	spImageComponent->SetRectScale(Vector3(ComponentIndex[0]["Scale"][0].GetFloat(), ComponentIndex[0]["Scale"][1].GetFloat(), ComponentIndex[0]["Scale"][2].GetFloat()));
	spImageComponent->SetRatio(ComponentIndex[0]["Ratio"].GetFloat());

	spImageComponent->SetOwnerGameObject((_spImageComponentDesc->m_wpOwnerGameObject).lock());
	if (EProjectionType::PERSPECTIVE == dynamic_pointer_cast<CUI>(spImageComponent->GetOwnerGameObject())->GetUIProjectionType())
	{
		spImageComponent->SetScreenRelativeCenter(Vector2(ComponentIndex[0]["RelativeCenter"][0].GetFloat(), ComponentIndex[0]["RelativeCenter"][1].GetFloat()));
	}

	if (EProjectionType::ORTHOGONAL == dynamic_pointer_cast<CUI>(spImageComponent->GetOwnerGameObject())->GetUIProjectionType())
	{
		spImageComponent->IsRelativeCenter(ComponentIndex[0]["IsRelative"].GetBool());
		if (true == spImageComponent->IsRelativeCenter())
		{
			spImageComponent->SetScreenRelativeCenter(Vector2(ComponentIndex[0]["RelativeCenter"][0].GetFloat(), ComponentIndex[0]["RelativeCenter"][1].GetFloat()));
		}
	}

	_spImageComponentDesc->m_wstrTextureFileName = CStringUtility::ToWideString(ComponentIndex[0]["Image Texture Name"].GetString());
	_spImageComponentDesc->m_eImageType = static_cast<EImageType>(ComponentIndex[0]["Image Type"].GetInt());

	spImageComponent->InitializeComponent(_spImageComponentDesc);

	return spImageComponent;
}

HRESULT Engine::CImageComponent::InitializeComponent(const std::shared_ptr<FComponentDesc>& _spComponentDesc)
{
	// rect component desc
	if (FAILED(CRectComponent::InitializeComponent(_spComponentDesc))) { return E_FAIL; }

	// image component desc
	shared_ptr<FImageComponentDesc> spImageComponentDesc = static_pointer_cast<FImageComponentDesc>(_spComponentDesc);
	m_wstrTextureFileName = spImageComponentDesc->m_wstrTextureFileName;
	m_eImageType = spImageComponentDesc->m_eImageType;

	// SRV : 1번 슬롯에 Base 텍스처를 넣음
	m_vecSRVs.push_back(CTextureHandler::GetInstance()->FindShaderResourceView(m_wstrTextureFileName, ETextureType::UI).Get());

	return S_OK;
}

HRESULT Engine::CImageComponent::BeginPlay()
{
	if (FAILED(CRectComponent::BeginPlay())) { return E_FAIL; }

	return S_OK;
}

int32 Engine::CImageComponent::PreTickComponent(float _fDeltaSeconds)
{
	switch (CRectComponent::PreTickComponent(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return 0;
}

int32 Engine::CImageComponent::TickComponent(float _fDeltaSeconds)
{
	switch (CRectComponent::TickComponent(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return 0;
}

int32 Engine::CImageComponent::PostTickComponent(float _fDeltaSeconds)
{
	switch (CRectComponent::PostTickComponent(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return 0;
}

HRESULT Engine::CImageComponent::EndPlay()
{
	if (FAILED(CRectComponent::EndPlay())) { return E_FAIL; }

	return S_OK;
}

void Engine::CImageComponent::Render(int32 _iShaderPath)
{
	CRectComponent::Render(_iShaderPath);
}

void Engine::CImageComponent::Release()
{
	CRectComponent::Release();
}

HRESULT Engine::CImageComponent::BindVariable()
{
	return CRectComponent::BindVariable();
}

void Engine::CImageComponent::SetTextureFileName(const std::wstring _wstrTextureFileName)
{
	if (m_vecSRVs.size() == 2)
	{
		m_vecSRVs[1] = nullptr;
	}

	m_vecSRVs[1] = CTextureHandler::GetInstance()->FindShaderResourceView(_wstrTextureFileName, ETextureType::UI).Get();
	m_wstrTextureFileName = _wstrTextureFileName;
}

