#include "pch.h"
#include "EffectComponent.h"
#include "MathUtility.h"

#include "EngineModule.h"
#include "RenderManager.h"

HRESULT Engine::CEffectComponent::InitializeComponent(const std::shared_ptr<FComponentDesc>& _spComponentDesc)
{
	// component
	if (FAILED(CComponent::InitializeComponent(_spComponentDesc))) { return E_FAIL; }

	// effect component
	shared_ptr<FEffectComponentDesc> spEffectComponentDesc = static_pointer_cast<FEffectComponentDesc>(_spComponentDesc);
	m_eEffectType = spEffectComponentDesc->m_eEffectType;

	// etc
	m_wpRenderManager = CEngineModule::GetInstance()->GetRenderManager();
	return S_OK;
}

HRESULT Engine::CEffectComponent::BeginPlay()
{
	return S_OK;
}

int32 Engine::CEffectComponent::PreTickComponent(float _fDeltaSeconds)
{
	return 0;
}

int32 Engine::CEffectComponent::TickComponent(float _fDeltaSeconds)
{
	return 0;
}

int32 Engine::CEffectComponent::PostTickComponent(float _fDeltaSeconds)
{
	// 렌더 매니저에 이펙트 컴포넌트를 추가합니다.
	m_wpRenderManager.lock()->AddComponent(shared_from_this(), m_eRenderType);
	return 0;
}

HRESULT Engine::CEffectComponent::EndPlay()
{
	return S_OK;
}

void Engine::CEffectComponent::Render(int32 _iShaderPath)
{
}

void Engine::CEffectComponent::Release()
{
	m_cpVariable_MeshInfo = nullptr;
	m_cpVariable_Textures = nullptr;
}

float Engine::CEffectComponent::EaseByType(EEasingType _eEasingType, float& _fStart, float& _fEnd, float _fRatio, float _fPower)
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

Vector3 Engine::CEffectComponent::EaseByType(EEasingType _eEasingType, Vector3& _vStart, Vector3& _vEnd, float _fRatio, float _fPower)
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

Vector4 Engine::CEffectComponent::EaseByType(EEasingType _eEasingType, Vector4& _vStart, Vector4& _vEnd, float _fRatio, float _fPower)
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
