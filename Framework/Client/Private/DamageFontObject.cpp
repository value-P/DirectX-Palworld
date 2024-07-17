#include "pch.h"
#include "DamageFontObject.h"

#include "StringUtility.h"
#include "MathUtility.h"

#include "TextComponent.h"

#include "UIManager.h"

std::shared_ptr<CDamageFontObject> Client::CDamageFontObject::Create(const std::shared_ptr<FDamageFontDesc>& _spDamageFontDesc)
{
    const std::shared_ptr<CDamageFontObject> spDamageFont = make_shared<CDamageFontObject>();
    ENSUREF(spDamageFont->Initialize(_spDamageFontDesc), L"Failed to initialize CDamageFontObject.");
    return spDamageFont;
}

std::shared_ptr<CDamageFontObject> Client::CDamageFontObject::CreateFromJson(const std::wstring& _wstrJsonFileName)
{
    const std::shared_ptr<CDamageFontObject> spDamageFont = make_shared<CDamageFontObject>();

    string strJsonFileName = CStringUtility::ToString(_wstrJsonFileName.c_str());
    ifstream ifsFilePath("../../Resource/UIs/" + strJsonFileName + ".json");
    if (!ifsFilePath.is_open())
    {
        return nullptr;
    }

    string strJsonFile((istreambuf_iterator<char>(ifsFilePath)), (istreambuf_iterator<char>()));

    rapidjson::Document doc;
    doc.Parse(strJsonFile.c_str());

    // 무조건적으로 패널하나의 정보를 로드합니다.
    int32 iPanelIndex = 0;
    spDamageFont->LoadJson(spDamageFont, doc, iPanelIndex);
    iPanelIndex++;

    rapidjson::Value& iPanelSize = doc["Child Panel Size"];

    // 이후 자식 패널의 사이즈를 저장한 걸 받아와서 있을 경우 for문을 돌려 패널을 만들어주고 AddChildGameObject() 해줍니다.
    for (int32 i = 0; i < iPanelSize.GetInt(); i++)
    {
        const std::shared_ptr<CPanel> spChildPanel = make_shared<CPanel>();
        if (nullptr != spDamageFont->GetPivotComponent())
        {
            spChildPanel->SetPivotComponent(spDamageFont->GetPivotComponent());
        }
        spChildPanel->LoadJson(spChildPanel, doc, iPanelIndex);
        // 다 로드한 내용을 토대로 자식오브젝트로 소속되게 해줍니다.
        spDamageFont->AddChildGameObject(spChildPanel);
        iPanelIndex++;
    }

    return spDamageFont;

    //const std::shared_ptr<CDamageFontObject> spDamageFont = static_pointer_cast<CDamageFontObject>(CPanel::CreateFromJson(_wstrJsonFileName));
    //if (FAILED(shared_ptr<FDamageFontDesc> spDamageFontDesc = static_pointer_cast<FDamageFontDesc>();
    //return spDamageFont;
}

HRESULT Client::CDamageFontObject::Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
    // panel desc
    if (FAILED(CPanel::Initialize(_spGameObjectDesc))) { return E_FAIL; }

    // damagefont desc
    shared_ptr<FDamageFontDesc> spDamageFontDesc = static_pointer_cast<FDamageFontDesc>(_spGameObjectDesc);

    return S_OK;
}

HRESULT Client::CDamageFontObject::BeginPlay()
{
    m_spBackText = FindComponent<CTextComponent>(L"DamageText");
    m_spFrontText = FindComponent<CTextComponent>(L"BaseText");

    m_wpUIManager = CUIManager::GetInstance();

    if (FAILED(CPanel::BeginPlay())) { return E_FAIL; }

    return S_OK;
}

int32 Client::CDamageFontObject::PreTick(float _fDeltaSeconds)
{
    if (!m_bActive) { return 0; }
    switch (CPanel::PreTick(_fDeltaSeconds))
    {
    case -1:	return -1;
    default:	break;
    }

    return 0;
}

int32 Client::CDamageFontObject::Tick(float _fDeltaSeconds)
{
    if (!m_bActive) { return 0; }
    switch (CPanel::Tick(_fDeltaSeconds))
    {
    case -1:	return -1;
    default:	break;
    }

    LerpDamageFont(_fDeltaSeconds);

    return 0;
}

int32 Client::CDamageFontObject::PostTick(float _fDeltaSeconds)
{
    if (!m_bActive) { return 0; }
    switch (CPanel::PostTick(_fDeltaSeconds))
    {
    case -1:	return -1;
    default:	break;
    }

    return 0;
}

HRESULT Client::CDamageFontObject::EndPlay()
{
    if (FAILED(CPanel::EndPlay())) { return E_FAIL; }

    return S_OK;
}

void Client::CDamageFontObject::Release()
{
    m_spBackText = nullptr;
    m_spFrontText = nullptr;

    CPanel::Release();
}

void Client::CDamageFontObject::SetDamageText(int32 _iDamage)
{
    m_spBackText->SetText(to_wstring(_iDamage));
    m_spFrontText->SetText(to_wstring(_iDamage));

    m_bStart = false;
}

void Client::CDamageFontObject::ActiveDamageFont(Vector3 _vPosition)
{
    m_vStartPos = _vPosition;
    // 켜진 처음에만 수행되어야 하는 것
    {
        // 알파값을 리셋시켜줍니다.
        m_spBackText->SetColor(Vector4(1.0f, 0.f, 0.f, 1.f));
        m_spFrontText->SetColor(Vector4(1.0f, 1.0f, 1.0f, 1.f));

        // 중점 위치를 잡아줍니다.
        SetPivotWorldPoint(_vPosition);
        // 랜덤 각도를 설정합니다.
        float fRandomAngle = CMathUtility::GetRandomFloat(0.f, 360.f);
        // 랜덤 각도에 따른 목표 지점을 설정합니다.
        float fDistance = CMathUtility::GetRandomFloat(0.2f, 0.35f);
        m_vEndPos.x = m_vStartPos.x + fDistance * cosf(fRandomAngle);
        m_vEndPos.y = m_vStartPos.y + fDistance * sinf(fRandomAngle);
        m_vEndPos.z = m_vStartPos.z;
    }

    m_bStart = true;
}

void Client::CDamageFontObject::LerpDamageFont(float _fDeltaSeconds)
{
    // 켜진 이후 계속 수행되어야 하는 것
    if (m_bStart)
    {
        m_fLerpTime += _fDeltaSeconds;
        // 중점에서 목표 지점까지 위치를 보간합니다.
        Vector3 vLerpPos = EaseByType(m_eEasingType, m_vStartPos, m_vEndPos, m_fLerpTime);
        // 텍스트 알파값을 보간합니다.
        float fStartAlpha = 1.0f;
        float fEndAlpha = 0.0f;
        float fAlpha = EaseByType(m_eEasingType, fStartAlpha, fEndAlpha, m_fLerpTime);

        SetPivotWorldPoint(vLerpPos);

        m_spBackText->SetColor(Vector4(1.0f, 0.f, 0.f, fAlpha));
        m_spFrontText->SetColor(Vector4(1.0f, 1.0f, 1.0f, fAlpha));

        if (1.0f < m_fLerpTime)
        {
            m_fLerpTime = 0.0f;
            m_bStart = false;
            // 알아서 꺼지고 풀에 반환하게 합니다.
            m_wpUIManager.lock()->ReturnDamageFont(static_pointer_cast<CDamageFontObject>(shared_from_this()));
        }
    }
}

void Client::CDamageFontObject::SetDamageFont(int32 _iDamage, Vector3 _vPosition)
{
    SetDamageText(_iDamage);
    ActiveDamageFont(_vPosition);
}

void Client::CDamageFontObject::SetDamageTextComponent(shared_ptr<CTextComponent> _spBase, shared_ptr<CTextComponent> _spDamage)
{
    m_spBackText = _spBase;
    m_spFrontText = _spDamage;

    m_wpUIManager = CUIManager::GetInstance();
}

float Client::CDamageFontObject::EaseByType(EEasingType _eEasingType, float& _fStart, float& _fEnd, float _fRatio, float _fPower)
{
    float fLerpedData = 0.f;

    switch (_eEasingType)
    {
    case EEasingType::EASE_NONE:		  fLerpedData = _fStart; break;
    case EEasingType::EASE_LINEAR:		  fLerpedData = lerp(_fStart, _fEnd, _fRatio); break;
    case EEasingType::EASE_IN_SINE:		  fLerpedData = CMathUtility::EaseInSine(_fStart, _fEnd, _fRatio); break;
    case EEasingType::EASE_OUT_SINE:	  fLerpedData = CMathUtility::EaseOutSine(_fStart, _fEnd, _fRatio); break;
    case EEasingType::EASE_INOUT_SINE:	  fLerpedData = CMathUtility::EaseInOutSine(_fStart, _fEnd, _fRatio); break;
    case EEasingType::EASE_IN_SQUARE:	  fLerpedData = CMathUtility::EaseInSquare(_fStart, _fEnd, _fRatio, _fPower); break;
    case EEasingType::EASE_OUT_SQUARE:	  fLerpedData = CMathUtility::EaseOutSquare(_fStart, _fEnd, _fRatio, _fPower); break;
    case EEasingType::EASE_INOUT_SQUARE:  fLerpedData = CMathUtility::EaseInOutSquare(_fStart, _fEnd, _fRatio, _fPower); break;
    case EEasingType::EASE_IN_CIRC:		  fLerpedData = CMathUtility::EaseInCirc(_fStart, _fEnd, _fRatio); break;
    case EEasingType::EASE_OUT_CIRC:	  fLerpedData = CMathUtility::EaseOutCirc(_fStart, _fEnd, _fRatio); break;
    case EEasingType::EASE_INOUT_CIRC:	  fLerpedData = CMathUtility::EaseInOutCirc(_fStart, _fEnd, _fRatio); break;
    case EEasingType::EASE_IN_BACK:		  fLerpedData = CMathUtility::EaseInBack(_fStart, _fEnd, _fRatio); break;
    case EEasingType::EASE_OUT_BACK:	  fLerpedData = CMathUtility::EaseOutBack(_fStart, _fEnd, _fRatio); break;
    case EEasingType::EASE_INOUT_BACK:	  fLerpedData = CMathUtility::EaseInOutBack(_fStart, _fEnd, _fRatio); break;
    case EEasingType::EASE_IN_ELASTIC:	  fLerpedData = CMathUtility::EaseInElastic(_fStart, _fEnd, _fRatio); break;
    case EEasingType::EASE_OUT_ELASTIC:	  fLerpedData = CMathUtility::EaseOutElastic(_fStart, _fEnd, _fRatio); break;
    case EEasingType::EASE_INOUT_ELASTIC: fLerpedData = CMathUtility::EaseInOutElastic(_fStart, _fEnd, _fRatio); break;
    case EEasingType::EASE_IN_BOUNCE:	  fLerpedData = CMathUtility::EaseInBounce(_fStart, _fEnd, _fRatio); break;
    case EEasingType::EASE_OUT_BOUNCE:	  fLerpedData = CMathUtility::EaseOutBounce(_fStart, _fEnd, _fRatio); break;
    case EEasingType::EASE_INOUT_BOUNCE:  fLerpedData = CMathUtility::EaseInOutBounce(_fStart, _fEnd, _fRatio); break;
    default: fLerpedData = _fStart; break;
    }

    return fLerpedData;
}

Vector3 Client::CDamageFontObject::EaseByType(EEasingType _eEasingType, Vector3& _vStart, Vector3& _vEnd, float _fRatio, float _fPower)
{
    Vector3 vLerpedData(0.0f);// = Vector3::Zero;

    switch (_eEasingType)
    {
    case EEasingType::EASE_NONE:		  vLerpedData = _vStart; break;
    case EEasingType::EASE_LINEAR:		  vLerpedData = Vector3::Lerp(_vStart, _vEnd, _fRatio); break;
    case EEasingType::EASE_IN_SINE:		  vLerpedData = CMathUtility::EaseInSine(_vStart, _vEnd, _fRatio); break;
    case EEasingType::EASE_OUT_SINE:	  vLerpedData = CMathUtility::EaseOutSine(_vStart, _vEnd, _fRatio); break;
    case EEasingType::EASE_INOUT_SINE:	  vLerpedData = CMathUtility::EaseInOutSine(_vStart, _vEnd, _fRatio); break;
    case EEasingType::EASE_IN_SQUARE:	  vLerpedData = CMathUtility::EaseInSquare(_vStart, _vEnd, _fRatio, _fPower); break;
    case EEasingType::EASE_OUT_SQUARE:	  vLerpedData = CMathUtility::EaseOutSquare(_vStart, _vEnd, _fRatio, _fPower); break;
    case EEasingType::EASE_INOUT_SQUARE:  vLerpedData = CMathUtility::EaseInOutSquare(_vStart, _vEnd, _fRatio, _fPower); break;
    case EEasingType::EASE_IN_CIRC:		  vLerpedData = CMathUtility::EaseInCirc(_vStart, _vEnd, _fRatio); break;
    case EEasingType::EASE_OUT_CIRC:	  vLerpedData = CMathUtility::EaseOutCirc(_vStart, _vEnd, _fRatio); break;
    case EEasingType::EASE_INOUT_CIRC:	  vLerpedData = CMathUtility::EaseInOutCirc(_vStart, _vEnd, _fRatio); break;
    case EEasingType::EASE_IN_BACK:		  vLerpedData = CMathUtility::EaseInBack(_vStart, _vEnd, _fRatio); break;
    case EEasingType::EASE_OUT_BACK:	  vLerpedData = CMathUtility::EaseOutBack(_vStart, _vEnd, _fRatio); break;
    case EEasingType::EASE_INOUT_BACK:	  vLerpedData = CMathUtility::EaseInOutBack(_vStart, _vEnd, _fRatio); break;
    case EEasingType::EASE_IN_ELASTIC:	  vLerpedData = CMathUtility::EaseInElastic(_vStart, _vEnd, _fRatio); break;
    case EEasingType::EASE_OUT_ELASTIC:	  vLerpedData = CMathUtility::EaseOutElastic(_vStart, _vEnd, _fRatio); break;
    case EEasingType::EASE_INOUT_ELASTIC: vLerpedData = CMathUtility::EaseInOutElastic(_vStart, _vEnd, _fRatio); break;
    case EEasingType::EASE_IN_BOUNCE:	  vLerpedData = CMathUtility::EaseInBounce(_vStart, _vEnd, _fRatio); break;
    case EEasingType::EASE_OUT_BOUNCE:	  vLerpedData = CMathUtility::EaseOutBounce(_vStart, _vEnd, _fRatio); break;
    case EEasingType::EASE_INOUT_BOUNCE:  vLerpedData = CMathUtility::EaseInOutBounce(_vStart, _vEnd, _fRatio); break;
    default: vLerpedData = _vStart; break;
    }

    return vLerpedData;
}

Vector4 Client::CDamageFontObject::EaseByType(EEasingType _eEasingType, Vector4& _vStart, Vector4& _vEnd, float _fRatio, float _fPower)
{
    Vector4 vLerpedData(0.0f);// = Vector4::Zero;

    switch (_eEasingType)
    {
    case EEasingType::EASE_NONE:		  vLerpedData = _vStart; break;
    case EEasingType::EASE_LINEAR:		  vLerpedData = Vector4::Lerp(_vStart, _vEnd, _fRatio); break;
    case EEasingType::EASE_IN_SINE:		  vLerpedData = CMathUtility::EaseInSine(_vStart, _vEnd, _fRatio); break;
    case EEasingType::EASE_OUT_SINE:	  vLerpedData = CMathUtility::EaseOutSine(_vStart, _vEnd, _fRatio); break;
    case EEasingType::EASE_INOUT_SINE:	  vLerpedData = CMathUtility::EaseInOutSine(_vStart, _vEnd, _fRatio); break;
    case EEasingType::EASE_IN_SQUARE:	  vLerpedData = CMathUtility::EaseInSquare(_vStart, _vEnd, _fRatio, _fPower); break;
    case EEasingType::EASE_OUT_SQUARE:	  vLerpedData = CMathUtility::EaseOutSquare(_vStart, _vEnd, _fRatio, _fPower); break;
    case EEasingType::EASE_INOUT_SQUARE:  vLerpedData = CMathUtility::EaseInOutSquare(_vStart, _vEnd, _fRatio, _fPower); break;
    case EEasingType::EASE_IN_CIRC:		  vLerpedData = CMathUtility::EaseInCirc(_vStart, _vEnd, _fRatio); break;
    case EEasingType::EASE_OUT_CIRC:	  vLerpedData = CMathUtility::EaseOutCirc(_vStart, _vEnd, _fRatio); break;
    case EEasingType::EASE_INOUT_CIRC:	  vLerpedData = CMathUtility::EaseInOutCirc(_vStart, _vEnd, _fRatio); break;
    case EEasingType::EASE_IN_BACK:		  vLerpedData = CMathUtility::EaseInBack(_vStart, _vEnd, _fRatio); break;
    case EEasingType::EASE_OUT_BACK:	  vLerpedData = CMathUtility::EaseOutBack(_vStart, _vEnd, _fRatio); break;
    case EEasingType::EASE_INOUT_BACK:	  vLerpedData = CMathUtility::EaseInOutBack(_vStart, _vEnd, _fRatio); break;
    case EEasingType::EASE_IN_ELASTIC:	  vLerpedData = CMathUtility::EaseInElastic(_vStart, _vEnd, _fRatio); break;
    case EEasingType::EASE_OUT_ELASTIC:	  vLerpedData = CMathUtility::EaseOutElastic(_vStart, _vEnd, _fRatio); break;
    case EEasingType::EASE_INOUT_ELASTIC: vLerpedData = CMathUtility::EaseInOutElastic(_vStart, _vEnd, _fRatio); break;
    case EEasingType::EASE_IN_BOUNCE:	  vLerpedData = CMathUtility::EaseInBounce(_vStart, _vEnd, _fRatio); break;
    case EEasingType::EASE_OUT_BOUNCE:	  vLerpedData = CMathUtility::EaseOutBounce(_vStart, _vEnd, _fRatio); break;
    case EEasingType::EASE_INOUT_BOUNCE:  vLerpedData = CMathUtility::EaseInOutBounce(_vStart, _vEnd, _fRatio); break;
    default: vLerpedData = _vStart; break;
    }

    return vLerpedData;
}
