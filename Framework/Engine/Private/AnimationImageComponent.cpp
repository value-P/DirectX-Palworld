#include "pch.h"
#include "AnimationImageComponent.h"

#include "EffectShader.h"
#include "TextureHandler.h"

#include "Panel.h"
#include "StringUtility.h"
#include "MathUtility.h"

std::shared_ptr<CAnimationImageComponent> Engine::CAnimationImageComponent::Create(const std::shared_ptr<FAnimationImageComponentDesc>& _spAnimationImageComponentDesc)
{
	const std::shared_ptr<CAnimationImageComponent> spAnimationImageComponent = make_shared<CAnimationImageComponent>();
	spAnimationImageComponent->InitializeComponent(_spAnimationImageComponentDesc);
	return spAnimationImageComponent;
}

std::shared_ptr<CAnimationImageComponent> Engine::CAnimationImageComponent::CreateFromJson(const std::shared_ptr<FAnimationImageComponentDesc>& _spAnimationImageComponentDesc, rapidjson::Document& _doc, int32& iPanelIndex, int32& iComponentIndex, int32& iKeyFrameIndex)
{
	const std::shared_ptr<CAnimationImageComponent> spAnimationImageComponent = make_shared<CAnimationImageComponent>();

	string strPanelValue = "Panels" + to_string(iPanelIndex);

	string strValue = "Components" + to_string(iComponentIndex);
	rapidjson::Value& ComponentIndex = _doc[strPanelValue.c_str()][strValue.c_str()][0];

	spAnimationImageComponent->SetUIType(static_cast<EUIType>(_doc[strPanelValue.c_str()][strValue.c_str()][0]["UI Type"].GetInt()));

	// 애니메이션 이미지 컴포넌트에 필요한 값들을 읽어 저장합니다.
	_spAnimationImageComponentDesc->m_wstrTextureFileName = CStringUtility::ToWideString(_doc[strPanelValue.c_str()][strValue.c_str()][0]["Animation Texture Name"].GetString());
	_spAnimationImageComponentDesc->m_bLoop = _doc[strPanelValue.c_str()][strValue.c_str()][0]["Loop"].GetBool();

	spAnimationImageComponent->SetEasingType(static_cast<EEasingType>(_doc[strPanelValue.c_str()][strValue.c_str()][0]["EasingType"].GetInt()));

	spAnimationImageComponent->SetOwnerGameObject((_spAnimationImageComponentDesc->m_wpOwnerGameObject).lock());
	if (EProjectionType::PERSPECTIVE == static_pointer_cast<CUI>(spAnimationImageComponent->GetOwnerGameObject())->GetUIProjectionType())
	{
		spAnimationImageComponent->SetScreenRelativeCenter(Vector2(_doc[strPanelValue.c_str()][strValue.c_str()][0]["RelativeCenter"][0].GetFloat(), _doc[strPanelValue.c_str()][strValue.c_str()][0]["RelativeCenter"][1].GetFloat()));
	}

	if (EProjectionType::ORTHOGONAL == static_pointer_cast<CUI>(spAnimationImageComponent->GetOwnerGameObject())->GetUIProjectionType())
	{
		spAnimationImageComponent->IsRelativeCenter(_doc[strPanelValue.c_str()][strValue.c_str()][0]["IsRelative"].GetBool());
		if (true == spAnimationImageComponent->IsRelativeCenter())
		{
			spAnimationImageComponent->SetScreenRelativeCenter(Vector2(_doc[strPanelValue.c_str()][strValue.c_str()][0]["RelativeCenter"][0].GetFloat(), _doc[strPanelValue.c_str()][strValue.c_str()][0]["RelativeCenter"][1].GetFloat()));
		}
	}

	if (EUIType::DYNAMIC_IMAGE == spAnimationImageComponent->GetUIType())
	{
		// 키프레임을 읽고 처음 키프레임만 이니셜라이즈 해준 뒤에 나머지는 addkeyframe()만 해줍니다.
		for (int32 i = 0; i < iKeyFrameIndex; i++)
		{
			// 키프레임의 value값을 설정합니다.
			string strKeyFrameValue = "KeyFrame" + to_string(i);

			// 키프레임 구조체를 만들고 값을 읽어 저장합니다.
			FImageAnimationFrameDesc fKeyFrameDesc;

			rapidjson::Value& KeyFrameIndex = _doc[strPanelValue.c_str()][strValue.c_str()][0]["KeyFrames"][i][strKeyFrameValue.c_str()];

			fKeyFrameDesc.fTime = KeyFrameIndex[0]["KeyFrame Time"].GetFloat();

			fKeyFrameDesc.vPosition.x = KeyFrameIndex[0]["KeyFrame Position"][0].GetFloat();
			fKeyFrameDesc.vPosition.y = KeyFrameIndex[0]["KeyFrame Position"][1].GetFloat();
			fKeyFrameDesc.vPosition.z = KeyFrameIndex[0]["KeyFrame Position"][2].GetFloat();

			fKeyFrameDesc.vRelativeCenter.x = KeyFrameIndex[0]["KeyFrame RelativeCenter"][0].GetFloat();
			fKeyFrameDesc.vRelativeCenter.y = KeyFrameIndex[0]["KeyFrame RelativeCenter"][1].GetFloat();
			fKeyFrameDesc.vRelativeCenter.z = 0.0f;

			fKeyFrameDesc.vScale.x = KeyFrameIndex[0]["KeyFrame Scale"][0].GetFloat();
			fKeyFrameDesc.vScale.y = KeyFrameIndex[0]["KeyFrame Scale"][1].GetFloat();
			fKeyFrameDesc.vScale.z = KeyFrameIndex[0]["KeyFrame Scale"][2].GetFloat();

			fKeyFrameDesc.fAngle = KeyFrameIndex[0]["KeyFrame Angle"].GetFloat();

			fKeyFrameDesc.vColor.x = KeyFrameIndex[0]["KeyFrame Color"][0].GetFloat();
			fKeyFrameDesc.vColor.y = KeyFrameIndex[0]["KeyFrame Color"][1].GetFloat();
			fKeyFrameDesc.vColor.z = KeyFrameIndex[0]["KeyFrame Color"][2].GetFloat();
			fKeyFrameDesc.vColor.w = KeyFrameIndex[0]["KeyFrame Color"][3].GetFloat();

			fKeyFrameDesc.fAlpha = KeyFrameIndex[0]["KeyFrame Alpha"].GetFloat();

			fKeyFrameDesc.fSpeed = KeyFrameIndex[0]["KeyFrame Speed"].GetFloat();

			_spAnimationImageComponentDesc->m_fImageAnimationDesc = fKeyFrameDesc;

			if (i == 0)
			{
				spAnimationImageComponent->InitializeComponent(_spAnimationImageComponentDesc);
			}
			else
			{
				spAnimationImageComponent->AddKeyFrame(fKeyFrameDesc);
			}
		}

		// 키프레임을 모두 추가하고 값을 저장해줍니다.
		spAnimationImageComponent->SetStartIndex(_doc[strPanelValue.c_str()][strValue.c_str()][0]["Start Index"].GetInt());
		spAnimationImageComponent->SetEndIndex(_doc[strPanelValue.c_str()][strValue.c_str()][0]["End Index"].GetInt());

		spAnimationImageComponent->SetIndex(spAnimationImageComponent->GetStartIndex());

		spAnimationImageComponent->SetOriginStartIndex(spAnimationImageComponent->GetStartIndex());
		spAnimationImageComponent->SetOriginEndIndex(spAnimationImageComponent->GetEndIndex());
	}

	if (EUIType::FRAME_IMAGE == spAnimationImageComponent->GetUIType())
	{

		spAnimationImageComponent->SetCenterPosition(Vector2(_doc[strPanelValue.c_str()][strValue.c_str()][0]["Center"][0].GetFloat(), _doc[strPanelValue.c_str()][strValue.c_str()][0]["Center"][1].GetFloat()));
		spAnimationImageComponent->SetRectPosition(Vector3(_doc[strPanelValue.c_str()][strValue.c_str()][0]["World Position"][0].GetFloat(), _doc[strPanelValue.c_str()][strValue.c_str()][0]["World Position"][1].GetFloat(), _doc[strPanelValue.c_str()][strValue.c_str()][0]["World Position"][2].GetFloat()));
		spAnimationImageComponent->SetRectScale(Vector3(_doc[strPanelValue.c_str()][strValue.c_str()][0]["Scale"][0].GetFloat(), _doc[strPanelValue.c_str()][strValue.c_str()][0]["Scale"][1].GetFloat(), _doc[strPanelValue.c_str()][strValue.c_str()][0]["Scale"][2].GetFloat()));
		spAnimationImageComponent->SetRatio(_doc[strPanelValue.c_str()][strValue.c_str()][0]["Ratio"].GetFloat());

		if (EProjectionType::PERSPECTIVE == static_pointer_cast<CUI>(spAnimationImageComponent->GetOwnerGameObject())->GetUIProjectionType())
		{
			spAnimationImageComponent->SetScreenRelativeCenter(Vector2(_doc[strPanelValue.c_str()][strValue.c_str()][0]["RelativeCenter"][0].GetFloat(), _doc[strPanelValue.c_str()][strValue.c_str()][0]["RelativeCenter"][1].GetFloat()));
		}

		if (EProjectionType::ORTHOGONAL == static_pointer_cast<CUI>(spAnimationImageComponent->GetOwnerGameObject())->GetUIProjectionType())
		{
			spAnimationImageComponent->IsRelativeCenter(_doc[strPanelValue.c_str()][strValue.c_str()][0]["IsRelative"].GetBool());
			if (true == spAnimationImageComponent->IsRelativeCenter())
			{
				spAnimationImageComponent->SetScreenRelativeCenter(Vector2(_doc[strPanelValue.c_str()][strValue.c_str()][0]["RelativeCenter"][0].GetFloat(), _doc[strPanelValue.c_str()][strValue.c_str()][0]["RelativeCenter"][1].GetFloat()));
			}
		}

		spAnimationImageComponent->SetRow(_doc[strPanelValue.c_str()][strValue.c_str()][0]["Row"].GetInt());
		spAnimationImageComponent->SetColumn(_doc[strPanelValue.c_str()][strValue.c_str()][0]["Coloumn"].GetInt());
		spAnimationImageComponent->SetNextTime(_doc[strPanelValue.c_str()][strValue.c_str()][0]["Next Time"].GetFloat());

		spAnimationImageComponent->InitializeComponent(_spAnimationImageComponentDesc);
	}
	return spAnimationImageComponent;
}

HRESULT Engine::CAnimationImageComponent::InitializeComponent(const std::shared_ptr<FComponentDesc>& _spComponentDesc)
{
	// rect component desc
	if (FAILED(CRectComponent::InitializeComponent(_spComponentDesc))) { return E_FAIL; }

	if (EUIType::DYNAMIC_IMAGE == m_eUIType)
	{
		// animationimage component desc
		shared_ptr<FAnimationImageComponentDesc> spAnimationComponentDesc = static_pointer_cast<FAnimationImageComponentDesc>(_spComponentDesc);
		m_wstrTextureFileName = spAnimationComponentDesc->m_wstrTextureFileName;
		m_bLoop = spAnimationComponentDesc->m_bLoop;
		m_bTool = spAnimationComponentDesc->m_bTool;
		m_bRelativeCenter = spAnimationComponentDesc->m_bRative;
		// 처음 생성하는 키프레임은 시간을 0으로 고정시킵니다.
		spAnimationComponentDesc->m_fImageAnimationDesc.fTime = 0.0f;

		// 처음 생성시 넣어주는 키프레임 구조체를 map 컨테이너에 넣어줍니다.
		AddKeyFrame(spAnimationComponentDesc->m_fImageAnimationDesc);

		if (EProjectionType::ORTHOGONAL == dynamic_pointer_cast<CUI>(m_wpOwnerGameObject.lock())->GetUIProjectionType())
		{
			SetRectTransform(ETransformType::POSITION, Vector4((spAnimationComponentDesc->m_fImageAnimationDesc.vPosition).x - 1280.f * 0.5f, -(spAnimationComponentDesc->m_fImageAnimationDesc.vPosition).y + 720.f * 0.5f, 0.0f, 1.f));
			//SetCenterPosition(Vector2((spAnimationComponentDesc->m_fImageAnimationDesc.vPosition).x, (spAnimationComponentDesc->m_fImageAnimationDesc.vPosition).y));
			SetRectScale(spAnimationComponentDesc->m_fImageAnimationDesc.vScale);
		}
		else if (EProjectionType::PERSPECTIVE == dynamic_pointer_cast<CUI>(m_wpOwnerGameObject.lock())->GetUIProjectionType())
		{
			SetRectTransform(ETransformType::POSITION, Vector4((spAnimationComponentDesc->m_fImageAnimationDesc.vPosition).x - 1280.f * 0.5f, -(spAnimationComponentDesc->m_fImageAnimationDesc.vPosition).y + 720.f * 0.5f, 0.0f, 1.f));

			//SetRectTransform(ETransformType::POSITION, Vector4(spAnimationComponentDesc->m_fImageAnimationDesc.vPosition.x, spAnimationComponentDesc->m_fImageAnimationDesc.vPosition.y, spAnimationComponentDesc->m_fImageAnimationDesc.vPosition.z, 1.f));
			SetRectScale(spAnimationComponentDesc->m_fImageAnimationDesc.vScale);

			m_iStartIndex = 0;
		}
	}
	if (EUIType::FRAME_IMAGE == m_eUIType)
	{
		// animationimage component desc
		shared_ptr<FAnimationImageComponentDesc> spAnimationComponentDesc = static_pointer_cast<FAnimationImageComponentDesc>(_spComponentDesc);

		m_wstrTextureFileName = spAnimationComponentDesc->m_wstrTextureFileName;
		m_bLoop = spAnimationComponentDesc->m_bLoop;
		m_bTool = spAnimationComponentDesc->m_bTool;
		m_bRelativeCenter = spAnimationComponentDesc->m_bRative;

		m_tShaderVariable_AnimationImage.vMinTexCoord = { 0.0f, 0.0f };
		m_tShaderVariable_AnimationImage.vMaxTexCoord = { 1.0f / static_cast<float>(m_iRow),  1.0f / static_cast<float>(m_iColumn) };
		m_fDuration = 0.0f;
	}

	// SRV : 1번 슬롯에 Base가 될 텍스처를 넣음
	m_vecSRVs.push_back(CTextureHandler::GetInstance()->FindShaderResourceView(m_wstrTextureFileName, ETextureType::UI).Get());

	// Bind Variable 변수 받아오기
	m_cpVariable_UiInfo_AnimationImage = m_wpEffectShader.lock()->GetRawVariableByName("g_tUiInfo_AnimationImage");

	return S_OK;
}

HRESULT Engine::CAnimationImageComponent::BeginPlay()
{
	if (FAILED(CRectComponent::BeginPlay())) { return E_FAIL; }

	return S_OK;
}

int32 Engine::CAnimationImageComponent::PreTickComponent(float _fDeltaSeconds)
{
	switch (CRectComponent::PreTickComponent(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return 0;
}

int32 Engine::CAnimationImageComponent::TickComponent(float _fDeltaSeconds)
{
	m_fDeltaSeconds = _fDeltaSeconds;

	//if (EUIType::DYNAMIC_IMAGE == m_eUIType)
	//{
	//	CaculateKeyFramesRelativePosition();

	//	if (!m_bReset)
	//	{
	//		CalculateLerp(_fDeltaSeconds);
	//		DelayTexture();
	//	}
	//}
	//else if (EUIType::FRAME_IMAGE == m_eUIType)
	//{
	//	CaculateFrameAnimation(_fDeltaSeconds);
	//}

	switch (CRectComponent::TickComponent(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return 0;
}

int32 Engine::CAnimationImageComponent::PostTickComponent(float _fDeltaSeconds)
{
	switch (CRectComponent::PostTickComponent(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return 0;
}

HRESULT Engine::CAnimationImageComponent::EndPlay()
{
	if (FAILED(CRectComponent::EndPlay())) { return E_FAIL; }

	return S_OK;
}

void Engine::CAnimationImageComponent::Render(int32 _iShaderPath)
{
	if (EUIType::DYNAMIC_IMAGE == m_eUIType)
	{
		CaculateKeyFramesRelativePosition();

		if (!m_bReset)
		{
			CalculateLerp(m_fDeltaSeconds);
			DelayTexture();
		}
	}
	else if (EUIType::FRAME_IMAGE == m_eUIType)
	{
		CaculateFrameAnimation(m_fDeltaSeconds);
	}

	CRectComponent::Render(_iShaderPath);
}

void Engine::CAnimationImageComponent::Release()
{
	m_cpVariable_UiInfo_AnimationImage = nullptr;
	CRectComponent::Release();
}

HRESULT Engine::CAnimationImageComponent::BindVariable()
{
	ENSURE(m_cpVariable_UiInfo_AnimationImage->SetRawValue(&m_tShaderVariable_AnimationImage, 0, sizeof(m_tShaderVariable_AnimationImage)));
	return CRectComponent::BindVariable();
}

HRESULT Engine::CAnimationImageComponent::AddKeyFrame(FImageAnimationFrameDesc _FImageAnimationDesc)
{
	float fKeyValue = _FImageAnimationDesc.fTime;

	// 만약 같은 시간 값이 입력되었다면 구조체를 교체해줍니다.
	for (auto& pair : m_vecKeyFrames)
	{
		if (fKeyValue == pair.first) { return E_FAIL; }
	}

	if (m_bTool)
	{
		shared_ptr<CRectComponent> spComponent = static_pointer_cast<CPanel>(m_wpOwnerGameObject.lock())->GetPivotComponent();

		// 피벗이 있고 내가 피벗이 아닐 시 상대좌표랑 기준좌표를 더해 절대좌표를 만들어줍니다.
		if (nullptr != spComponent && shared_from_this() != spComponent &&
			true == m_bRelativeCenter && EProjectionType::ORTHOGONAL == static_pointer_cast<CPanel>(m_wpOwnerGameObject.lock())->GetUIProjectionType())
		{
			_FImageAnimationDesc.vRelativeCenter.x = _FImageAnimationDesc.vPosition.x - spComponent->GetCenterPosition().x;
			_FImageAnimationDesc.vRelativeCenter.y = _FImageAnimationDesc.vPosition.y - spComponent->GetCenterPosition().y;
		}
		// 월드 ui일 시 계산을 수행합니다.
		if (EProjectionType::PERSPECTIVE == static_pointer_cast<CPanel>(m_wpOwnerGameObject.lock())->GetUIProjectionType() &&
			shared_from_this() != spComponent)
		{
			_FImageAnimationDesc.vRelativeCenter.x = _FImageAnimationDesc.vPosition.x - spComponent->GetCenterPosition().x;
			_FImageAnimationDesc.vRelativeCenter.y = _FImageAnimationDesc.vPosition.y - spComponent->GetCenterPosition().y;
		}
	}

	SetCenterPosition(Vector2(_FImageAnimationDesc.vPosition.x, _FImageAnimationDesc.vPosition.y));

	m_vecKeyFrames.push_back({ fKeyValue, _FImageAnimationDesc });

	// 추가하고 정렬을 수행해줍니다.
	std::sort(m_vecKeyFrames.begin(), m_vecKeyFrames.end(), CompareKey);

	m_iCurIndex = m_iStartIndex;
	m_fAnimationRunTime = 0.0f;
	m_fKeyFrameRunTime = 0.0f;

	SetEndIndex(static_cast<int32>(m_vecKeyFrames.size()) - 1);

	return S_OK;
}

HRESULT Engine::CAnimationImageComponent::SubtractKeyFrame(const int _iIndex)
{
	// pair의 first 값과 들어온 _KeyValue의 값을 비교하여 해당 객체를 찾고 삭제합니다.
	for (auto pair = m_vecKeyFrames.begin(); pair != m_vecKeyFrames.end(); )
	{
		if (pair->first == m_vecKeyFrames[_iIndex].first)
		{
			pair = m_vecKeyFrames.erase(pair);
			break;
		}
		else { pair++; }
	}

	// 삭제하고 정렬을 수행해줍니다.
	std::sort(m_vecKeyFrames.begin(), m_vecKeyFrames.end(), CompareKey);

	// 키프레임을 초기화해줍니다.
	m_iCurIndex = m_iStartIndex;
	m_fAnimationRunTime = 0.0f;
	m_fKeyFrameRunTime = 0.0f;

	SetEndIndex(static_cast<int32>(m_vecKeyFrames.size()) - 1);

	return S_OK;
}

HRESULT Engine::CAnimationImageComponent::ReplaceKeyFrame(FImageAnimationFrameDesc _FImageAnimationDesc)
{
	for (auto& pair : m_vecKeyFrames)
	{
		// 시간 값이 같은 경우 원래 있던 구조체를 매개변수로 들어온 구조체로 바꿔줍니다.
		if (_FImageAnimationDesc.fTime == pair.first)
		{
			shared_ptr<CRectComponent> spComponent = static_pointer_cast<CPanel>(m_wpOwnerGameObject.lock())->GetPivotComponent();

			// 피벗이 있고 내가 피벗이 아닐 시 상대좌표랑 기준좌표를 더해 절대좌표를 만들어줍니다.
			if (nullptr != spComponent && shared_from_this() != spComponent &&
				true == m_bRelativeCenter && EProjectionType::ORTHOGONAL == static_pointer_cast<CPanel>(m_wpOwnerGameObject.lock())->GetUIProjectionType())
			{
				_FImageAnimationDesc.vRelativeCenter.x = _FImageAnimationDesc.vPosition.x - spComponent->GetCenterPosition().x;
				_FImageAnimationDesc.vRelativeCenter.y = _FImageAnimationDesc.vPosition.y - spComponent->GetCenterPosition().y;
			}
			// 월드 ui일 시 계산을 수행합니다.
			if (EProjectionType::PERSPECTIVE == static_pointer_cast<CPanel>(m_wpOwnerGameObject.lock())->GetUIProjectionType() &&
				shared_from_this() != spComponent)
			{
				_FImageAnimationDesc.vRelativeCenter.x = _FImageAnimationDesc.vPosition.x - spComponent->GetCenterPosition().x;
				_FImageAnimationDesc.vRelativeCenter.y = _FImageAnimationDesc.vPosition.y - spComponent->GetCenterPosition().y;
				//_FImageAnimationDesc.vPosition.x = _FImageAnimationDesc.vRelativeCenter.x + spComponent->GetWorldToScreenPosition().x;
				//_FImageAnimationDesc.vPosition.y = _FImageAnimationDesc.vRelativeCenter.y + spComponent->GetWorldToScreenPosition().y;
				//_FImageAnimationDesc.vPosition.z = 0.0f;
			}
			SetCenterPosition(Vector2(_FImageAnimationDesc.vPosition.x, _FImageAnimationDesc.vPosition.y));

			pair.second = _FImageAnimationDesc;
		}
	}

	std::sort(m_vecKeyFrames.begin(), m_vecKeyFrames.end(), CompareKey);

	m_iCurIndex = m_iStartIndex;
	m_fAnimationRunTime = 0.0f;
	m_fKeyFrameRunTime = 0.0f;

	return S_OK;
}

HRESULT Engine::CAnimationImageComponent::CaculateKeyFramesRelativePosition()
{
	shared_ptr<CRectComponent> spComponent = static_pointer_cast<CPanel>(m_wpOwnerGameObject.lock())->GetPivotComponent();

	for (auto& iter : m_vecKeyFrames)
	{
		// 피벗이 있고 내가 피벗이 아닐 시 상대좌표랑 기준좌표를 더해 절대좌표를 만들어줍니다.
		if (nullptr != spComponent && shared_from_this() != spComponent &&
			true == m_bRelativeCenter && EProjectionType::ORTHOGONAL == static_pointer_cast<CPanel>(m_wpOwnerGameObject.lock())->GetUIProjectionType())
		{
			iter.second.vPosition.x = iter.second.vRelativeCenter.x + spComponent->GetCenterPosition().x;
			iter.second.vPosition.y = iter.second.vRelativeCenter.y + spComponent->GetCenterPosition().y;
			iter.second.vPosition.z = 0.0f;
		}
		// 월드 ui일 시 계산을 수행합니다.
		if (EProjectionType::PERSPECTIVE == static_pointer_cast<CPanel>(m_wpOwnerGameObject.lock())->GetUIProjectionType() &&
			shared_from_this() != spComponent)
		{
			iter.second.vPosition.x = iter.second.vRelativeCenter.x + spComponent->GetWorldToScreenPosition().x;
			iter.second.vPosition.y = iter.second.vRelativeCenter.y + spComponent->GetWorldToScreenPosition().y;
			iter.second.vPosition.z = 0.0f;
		}
	}

	return S_OK;
}

// 맵에 저장된 키프레임들을 보간해줍니다.
void Engine::CAnimationImageComponent::CalculateLerp(float _fDeltaSeconds)
{
	int32 iVecSize = static_cast<int32>(m_vecKeyFrames.size());

	// 맵에 보관된 값이 1 이하일 경우 수행하지않습니다.
	if (iVecSize > 1)
	{
		// 전체 수행 시간을 저장해줍니다.
		m_fAnimationRunTime += _fDeltaSeconds;
		// 키프레임 별 수행 시간을 저장해줍니다.
		m_fKeyFrameRunTime += _fDeltaSeconds;

		float fRatio = m_fKeyFrameRunTime / (m_vecKeyFrames[m_iCurIndex + 1].first - m_vecKeyFrames[m_iCurIndex].first);

		Vector4 vColor = EaseByType(m_eEasingType, m_vecKeyFrames[m_iCurIndex].second.vColor, m_vecKeyFrames[m_iCurIndex + 1].second.vColor, fRatio);
		Vector3 vPosition = EaseByType(m_eEasingType, m_vecKeyFrames[m_iCurIndex].second.vPosition, m_vecKeyFrames[m_iCurIndex + 1].second.vPosition, fRatio);
		Vector3 vScale = EaseByType(m_eEasingType, m_vecKeyFrames[m_iCurIndex].second.vScale, m_vecKeyFrames[m_iCurIndex + 1].second.vScale, fRatio);
		float fAngle = EaseByType(m_eEasingType, m_vecKeyFrames[m_iCurIndex].second.fAngle, m_vecKeyFrames[m_iCurIndex + 1].second.fAngle, fRatio);
		float fAlpha = EaseByType(m_eEasingType, m_vecKeyFrames[m_iCurIndex].second.fAlpha, m_vecKeyFrames[m_iCurIndex + 1].second.fAlpha, fRatio);

		if (1.0f <= fRatio)
		{
			vColor = m_vecKeyFrames[m_iCurIndex + 1].second.vColor;
			vPosition = m_vecKeyFrames[m_iCurIndex + 1].second.vPosition;
			vScale = m_vecKeyFrames[m_iCurIndex + 1].second.vScale;
			//matRotation = m_vecKeyFrames[m_iCurIndex + 1].second.matRotation;
			fAlpha = m_vecKeyFrames[m_iCurIndex + 1].second.fAlpha;
			fAngle = m_vecKeyFrames[m_iCurIndex + 1].second.fAngle;
		}

		SetRectScale(vScale);
		SetRectAngle(fAngle);
		if (EUIType::DYNAMIC_IMAGE == m_eUIType)
		{
			Matrix matRotation = ChangeRotationMatrix(m_fRectAngle);
			SetRectRotate(matRotation);
		}
		//SetRectRotate(matRotation);

		// 보간을 수행한 각 값들을 세팅해줍니다.
		SetRectTransform(ETransformType::POSITION, Vector4(vPosition.x - 1280.f * 0.5f, -vPosition.y + 720.f * 0.5f, 0.0f, 1.0f));

		m_vWorldToScreenPosition.x = vPosition.x;
		m_vWorldToScreenPosition.y = vPosition.y;

		m_vCenter.x = vPosition.x;
		m_vCenter.y = vPosition.y;
		m_fAlpha = fAlpha;
		m_vColor = vColor;

		// 시간이 지나면 다음 키프레임으로 넘어갑니다.
		if (m_vecKeyFrames[m_iCurIndex + 1].first <= m_fAnimationRunTime)
		{
			if (m_iCurIndex < m_iEndIndex - 1)//iVecSize - 2)
			{
				m_fKeyFrameRunTime = 0.0f;
				m_iCurIndex++;
			}
			else
			{
				m_bFinish = true;
			}
		}
	}

	if (m_bLoop)
	{
		// 전체 수행 시간이 map 컨테이너의 마지막 시간값보다 커지면 전체 수행시간을 초기화시켜줍니다. (루프일 경우)
		if (iVecSize > 1)
		{
			if (m_vecKeyFrames[m_iEndIndex].first < m_fAnimationRunTime)
			{
				m_iLoopCount++;
				m_bFinish = false;
				m_fAnimationRunTime = m_vecKeyFrames[m_iStartIndex].first;
				m_fKeyFrameRunTime = 0.0f;
				m_iCurIndex = m_iStartIndex;
			}
		}
	}
}

void Engine::CAnimationImageComponent::ResetWorldPosition(const Vector2& _vCenter)
{
	// 현재 키프레임에 들어가있는 값은 월드포지션이므로 받아온 센터점을 월드포지션으로 변화시켜줍니다.
	Vector3 vPosition = GetRectWorldPosition(Vector3(_vCenter.x, _vCenter.y, 0.f));

	// 변환한 월드포지션으로 모든 키프레임의 포지션을 초기화시켜줍니다.
	for (auto& pair : m_vecKeyFrames)
	{
		pair.second.vPosition = vPosition;
	}
}

pair<float, FImageAnimationFrameDesc> Engine::CAnimationImageComponent::FindKeyFrame(const int _iIndex)
{
	for (auto& pair : m_vecKeyFrames)
	{
		if (pair.first == m_vecKeyFrames[_iIndex].first)
		{
			return pair;
		}
	}

	return { 100.0f, {} };
}

bool Engine::CAnimationImageComponent::CompareKey(pair<float, FImageAnimationFrameDesc>& _prFirst, pair<float, FImageAnimationFrameDesc>& _prSecond)
{
	return _prFirst.first < _prSecond.first;
}

void Engine::CAnimationImageComponent::ResetFrame()
{
	m_iLoopCount = 0;
	m_bFinish = false;
	m_iCurIndex = 0;
	m_fAnimationRunTime = m_vecKeyFrames[m_iCurIndex].first;
	m_fKeyFrameRunTime = 0.0f;
}

void Engine::CAnimationImageComponent::SetStartEndIndex(int32 _iStartIndex, int32 _iEndIndex)
{
	if (_iEndIndex <= _iStartIndex)
	{
		m_iStartIndex = m_iEndIndex - 1;
	}
	else if (_iStartIndex <= 0)
	{
		m_iStartIndex = 0;
	}
	else
	{
		m_iStartIndex = _iStartIndex;
	}

	if (_iEndIndex <= 0)
	{
		m_iEndIndex = 1;
	}
	else if (_iEndIndex > m_vecKeyFrames.size() - 1)
	{
		_iEndIndex = static_cast<int32>(m_vecKeyFrames.size()) - 1;
	}
	else
	{
		m_iEndIndex = _iEndIndex;
	}

	m_iCurIndex = m_iStartIndex;
	//m_iStartIndexm_iCurIndex = m_iStartIndex;
	m_fAnimationRunTime = m_vecKeyFrames[m_iStartIndex].first;
	m_fKeyFrameRunTime = 0.0f;
	m_bFinish = false;
	m_iLoopCount = 0;

	// 여기서 시작 인덱스의 구조체 정보로 업데이트 시켜줍니다.
	SetCenterPosition(Vector2(m_vecKeyFrames[m_iStartIndex].second.vPosition.x, m_vecKeyFrames[m_iStartIndex].second.vPosition.y));
}

void Engine::CAnimationImageComponent::ResetAnimationValues()
{
	m_iStartIndex = m_iOriginStartIndex;
	m_iEndIndex = m_iOriginEndIndex;

	m_fKeyFrameRunTime = 0.0f;
	m_fAnimationRunTime = m_vecKeyFrames[m_iStartIndex].first;
	m_iCurIndex = m_iStartIndex;
	m_bFinish = false;
	m_iLoopCount = 0;
}

void Engine::CAnimationImageComponent::CaculateFrameAnimation(float _fDeltaSeconds)
{
	m_fDuration += _fDeltaSeconds;
	float iWidth = 1.0f / static_cast<float>(m_iRow);
	float iHeight = 1.0f / static_cast<float>(m_iColumn);
	// texcoord 값을 변경해줍니다.
	if (m_fDuration >= m_fNextTime)
	{
		m_tShaderVariable_AnimationImage.vMinTexCoord.x += 1.0f / static_cast<float>(m_iRow);
		m_tShaderVariable_AnimationImage.vMaxTexCoord.x += 1.0f / static_cast<float>(m_iRow);

		if (m_tShaderVariable_AnimationImage.vMaxTexCoord.x > 1.0f)
		{
			m_tShaderVariable_AnimationImage.vMinTexCoord.x = 0.0f;
			m_tShaderVariable_AnimationImage.vMinTexCoord.y += 1.0f / static_cast<float>(m_iColumn);

			m_tShaderVariable_AnimationImage.vMaxTexCoord.x = 1.0f / static_cast<float>(m_iRow);
			m_tShaderVariable_AnimationImage.vMaxTexCoord.y += 1.0f / static_cast<float>(m_iColumn);
		}

		if (m_tShaderVariable_AnimationImage.vMaxTexCoord.y > 1.0f)
		{
			m_tShaderVariable_AnimationImage.vMinTexCoord = { 0.0f, 0.0f };
			m_tShaderVariable_AnimationImage.vMaxTexCoord = { 1.0f / static_cast<float>(m_iRow),  1.0f / static_cast<float>(m_iColumn) };
		}

		m_fDuration = 0.0f;
	}
}

void Engine::CAnimationImageComponent::SetRowCoulmn(int32 _iRow, int32 _iColumn, float _fNextTime)
{
	m_fNextTime = _fNextTime;

	m_iRow = _iRow;
	m_iColumn = _iColumn;

	m_tShaderVariable_AnimationImage.vMinTexCoord = { 0.0f, 0.0f };
	m_tShaderVariable_AnimationImage.vMaxTexCoord = { 1.0f / static_cast<float>(m_iRow),  1.0f / static_cast<float>(m_iColumn) };
	m_fDuration = 0.0f;
}

void Engine::CAnimationImageComponent::SetStartReset(bool _bReset)
{
	m_bReset = _bReset;
	if (m_bReset)
	{
		m_fAnimationRunTime = m_vecKeyFrames[m_iStartIndex].first;
		m_fKeyFrameRunTime = 0.0f;
		m_iCurIndex = m_iStartIndex;
		m_iLoopCount = 0;
		m_bFinish = false;
	}
}

void Engine::CAnimationImageComponent::SetTextureFileName(const std::wstring& _wstrTextureName)
{
	if (m_vecSRVs.size() == 2)
	{
		m_vecSRVs[1] = nullptr;
	}

	// 여기 터지면 -동연-을 찾아주세요.
	m_vecSRVs[1] = CTextureHandler::GetInstance()->FindShaderResourceView(_wstrTextureName, ETextureType::UI).Get();
	m_wstrTextureFileName = _wstrTextureName;
}

void Engine::CAnimationImageComponent::SetTextureFileName(const std::wstring& _wstrTextureName, bool _bDelay)
{
	m_wstrTextureFileName = _wstrTextureName;
	m_bDelay = _bDelay;
}

void Engine::CAnimationImageComponent::SetStartIndex(int32 _iStartIndex)
{
	if (m_iEndIndex <= _iStartIndex)
	{
		m_iStartIndex = m_iEndIndex - 1;
	}
	else if (_iStartIndex <= 0)
	{
		m_iStartIndex = 0;
	}
	else
	{
		m_iStartIndex = _iStartIndex;
	}
	m_bFinish = false;
	m_iLoopCount = 0;
	//m_fAnimationRunTime = m_vecKeyFrames[m_iStartIndex].first;
	//m_fKeyFrameRunTime = 0.0f;
	//m_iCurIndex = m_iStartIndex;
}

void Engine::CAnimationImageComponent::SetEndIndex(int32 _iEndIndex)
{
	if (_iEndIndex <= 0)
	{
		m_iEndIndex = 1;
	}
	else if (_iEndIndex > m_vecKeyFrames.size() - 1)
	{
		_iEndIndex = static_cast<int32>(m_vecKeyFrames.size()) - 1;
	}
	else
	{
		m_iEndIndex = _iEndIndex;
	}
	m_bFinish = false;
	m_iLoopCount = 0;
}


float Engine::CAnimationImageComponent::EaseByType(EEasingType _eEasingType, float& _fStart, float& _fEnd, float _fRatio, float _fPower)
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

Vector3 Engine::CAnimationImageComponent::EaseByType(EEasingType _eEasingType, Vector3& _vStart, Vector3& _vEnd, float _fRatio, float _fPower)
{
	Vector3 vLerpedData = Vector3::Zero;

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

Vector4 Engine::CAnimationImageComponent::EaseByType(EEasingType _eEasingType, Vector4& _vStart, Vector4& _vEnd, float _fRatio, float _fPower)
{
	Vector4 vLerpedData = Vector4::Zero;

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

void Engine::CAnimationImageComponent::DelayTexture()
{
	if (true == m_bDelay)
	{
		if (m_vecSRVs.size() == 2)
		{
			m_vecSRVs[1] = nullptr;
		}

		// 여기 터지면 -동연-을 찾아주세요.
		m_vecSRVs[1] = CTextureHandler::GetInstance()->FindShaderResourceView(m_wstrTextureFileName, ETextureType::UI).Get();
		m_bDelay = false;
	}
}
