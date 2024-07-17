#include "pch.h"
#include "Creature.h"

#include "EngineModule.h"
#include "ClientLoop.h"
#include "ClientModule.h"

#include "NotifyManager.h"
#include "EffectManager.h"
#include "UIManager.h"

#include "MathUtility.h"
#include "SoundManager.h"
#include "StringUtility.h"

#include "SphereComponent.h"
#include "BoxComponent.h"
#include "MeshComponent.h"
#include "Bone.h"
#include "ClientLoop.h"

#include "DamageFontObject.h"
#include "Monster.h"

HRESULT Client::CCreature::Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	// character desc
	if (FAILED(CCharacter::Initialize(_spGameObjectDesc))) { return E_FAIL; }

	// creature desc
	shared_ptr<FCreatureDesc> spCreatureDesc = static_pointer_cast<FCreatureDesc>(_spGameObjectDesc);
	m_eCreatureType = spCreatureDesc->m_eCreatureType;

	if (spCreatureDesc->m_fCurHp.has_value()) { m_fCurHp = spCreatureDesc->m_fCurHp.value(); }

	if (spCreatureDesc->m_fMaxHp.has_value()) { m_fMaxHp = spCreatureDesc->m_fMaxHp.value(); }

	if (spCreatureDesc->m_fCurExp.has_value()) { m_fCurExp = spCreatureDesc->m_fCurExp.value(); }

	if (spCreatureDesc->m_fMaxExp.has_value()) { m_fMaxExp = spCreatureDesc->m_fMaxExp.value(); }

	if (spCreatureDesc->m_fCurHungerGauge.has_value()) { m_fCurHungerGauge = spCreatureDesc->m_fCurHungerGauge.value(); }

	if (spCreatureDesc->m_fAttackPower.has_value()) { m_fAttackPower = spCreatureDesc->m_fAttackPower.value(); }

	if (spCreatureDesc->m_fDefensePower.has_value()) { m_fDefensePower = spCreatureDesc->m_fDefensePower.value(); }

	// etc
	m_wpNotifyManager = CClientModule::GetInstance()->GetNotifyManager();
	m_wpEffectManager = CEffectManager::GetInstance();
	m_wpSoundManager = CEngineModule::GetInstance()->GetSoundManager();
	m_wpUIManager = CUIManager::GetInstance();

	return S_OK;
}

HRESULT Client::CCreature::BeginPlay()
{
	if (FAILED(CCharacter::BeginPlay())) { return E_FAIL; }

	return S_OK;
}

int32 Client::CCreature::PreTick(float _fDeltaSeconds)
{
	switch (CCharacter::PreTick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}
	return 0;
}

int32 Client::CCreature::Tick(float _fDeltaSeconds)
{
	switch (CCharacter::Tick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}
	return 0;
}

int32 Client::CCreature::PostTick(float _fDeltaSeconds)
{
	switch (CCharacter::PostTick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}
	return 0;
}

HRESULT Client::CCreature::EndPlay()
{
	if (FAILED(CCharacter::EndPlay())) { return E_FAIL; }

	return S_OK;
}

void Client::CCreature::Release()
{
	CCharacter::Release();
}

void Client::CCreature::IncreaseHungerGauge(float _fDeltaSeconds)
{
	m_fCurHungerGauge += CClientLoop::s_spClientDataSheet->k_fIncreaseHungerGaugePerSeconds * _fDeltaSeconds;

	if (CClientLoop::s_spClientDataSheet->k_fMaxHungerGauge <= m_fCurHungerGauge)
	{
		m_fCurHungerGauge = CClientLoop::s_spClientDataSheet->k_fMaxHungerGauge;
	}
}

void Client::CCreature::DecreaseHungerGauge(float _fDeltaSeconds)
{
	m_fCurHungerGauge -= CClientLoop::s_spClientDataSheet->k_fDecreaseHungerGaugePerSeconds * _fDeltaSeconds * 10.0f;

	if (m_fCurHungerGauge < 1.0f)
	{
		m_fCurHungerGauge = 1.0f;
	}
}

void Client::CCreature::TakeDamage(const std::shared_ptr<CGameObject>& _spGameObject, float _fDamage)
{
	float fBaseDamage = _fDamage - m_fDefensePower;
	if (fBaseDamage < 0.f) { fBaseDamage = 0.f; }

	float fTotalDamage = fBaseDamage + CMathUtility::GetRandomFloat(3.f, 5.f);

	m_fCurHp -= fTotalDamage;

	if (m_fCurHp <= 0.0f)
	{
		m_fCurHp = 0.0f;
		m_bDead = true;
	}

	Vector3 vPosition(0.0f);
	if (ECreatureType::PLAYER == m_eCreatureType)
	{
		vPosition = GetPosition().value();
	}
	else if (ECreatureType::PAL == m_eCreatureType)
	{
		std::shared_ptr<CMonster> spMonster = static_pointer_cast<CMonster>(shared_from_this());
		if (EMonsterType::DREAM_DEMON == spMonster->GetMonsterType())
		{
			vPosition = GetPosition().value() + static_pointer_cast<CBoxComponent>(spMonster->GetBodyColliderComponent())->GetLocalPosition();
		}
		else
		{
			vPosition = GetPosition().value() + static_pointer_cast<CSphereComponent>(spMonster->GetBodyColliderComponent())->GetLocalPosition();
		}
	}

	std::shared_ptr<CDamageFontObject> spDamageFont = m_wpUIManager.lock()->GetUsingDamageFontUI();
	spDamageFont->SetDamageFont((int32)fTotalDamage, vPosition);
}

void Client::CCreature::AnimNotifyCall(const list<FAnimNotifyData>& _lstAnimNotify)
{
	for (FAnimNotifyData tData : _lstAnimNotify)
	{
		switch (tData.m_eNotifyType)
		{
		case ENotifyType::COLLISION:
			ColliderOn(tData);
			break;

		case ENotifyType::SOUND:
			PlaySoundWithID(tData);
			break;

		case ENotifyType::EFFECT:
			PlayEffect(tData);
			break;

		case ENotifyType::FUNC:
			PlayFunction(tData);
			break;

		default:
			break;
		}
	}
}

void Client::CCreature::ColliderOn(const FAnimNotifyData& _tNotifyData)
{
	for (int32 iCnt = 0; iCnt < _tNotifyData.m_tCollisionNotify.m_iColliderCount; ++iCnt)
	{
		// 위치 조정
		Matrix offsetBoneMatrix = m_spMeshComponent->GetBonePtr(_tNotifyData.m_tCollisionNotify.m_iAttachBoneIndex)->GetCombinedTransformationMatrix();
		Matrix relativeMatrix = Matrix::CreateTranslation(_tNotifyData.m_tCollisionNotify.m_vecColliderRelativeLocation[iCnt]);

		ECollisionLayer eColLayer;
		ECollisionFlag eColFlag;

		if (m_eCreatureType == ECreatureType::PLAYER) { eColLayer = ECollisionLayer::PLAYER_WEAPON; eColFlag = ECollisionFlag::PLAYER_WEAPON; }
		else if (m_eCreatureType == ECreatureType::PAL) { eColLayer = ECollisionLayer::MONSTER_WEAPON; eColFlag = ECollisionFlag::MONSTER_WEAPON; }
		else { eColLayer = ECollisionLayer::ENUM_END; eColFlag = ECollisionFlag::ENUM_END; }

		relativeMatrix = relativeMatrix * CMathUtility::GetScaledKilledMatrix(offsetBoneMatrix) * GetTransformMatrix().value();

		m_wpNotifyManager.lock()->NotifyCollison
		(
			shared_from_this(),
			relativeMatrix.Translation(),
			_tNotifyData.m_tCollisionNotify.m_vecColliderRadius[iCnt],
			_tNotifyData.m_fDuration,
			eColFlag,
			eColLayer
		);
	}
}

void Client::CCreature::PlaySoundWithID(const FAnimNotifyData& _tNotifyData)
{
	FSoundNotifyData tSoundNotifyData = _tNotifyData.m_tSoundNotify;

	m_wpSoundManager.lock()->PlaySoundEx(CStringUtility::ToString(tSoundNotifyData.m_wstrSoundID), GetPosition().value());
}

void Client::CCreature::PlayEffect(const FAnimNotifyData& _tNotifyData)
{
	FEffectNotifyData tEffectNotifyData = _tNotifyData.m_tEffectNotify;
	shared_ptr<CBone> spBone = m_spMeshComponent->GetBonePtr(tEffectNotifyData.m_iAttachBoneIndex);

	if (tEffectNotifyData.m_bUseFollowBone)
	{
		m_wpEffectManager.lock()->ActivateEffectCombined(tEffectNotifyData.m_wstrEffectName, shared_from_this(), spBone, tEffectNotifyData.m_vRelativeLocation);
	}
	else
	{
		// 위치 조정
		Matrix offsetBoneMatrix = CMathUtility::GetScaledKilledMatrix(spBone->GetCombinedTransformationMatrix());
		Matrix relativeMatrix = Matrix::CreateTranslation(tEffectNotifyData.m_vRelativeLocation);
		Matrix matWorld = relativeMatrix * offsetBoneMatrix * GetTransformMatrix().value();
		m_wpEffectManager.lock()->ActivateEffect(tEffectNotifyData.m_wstrEffectName, shared_from_this(), matWorld.Translation());
	}
}

void Client::CCreature::PlayFunction(const FAnimNotifyData& _tNotifyData)
{
	if (m_umapFunctions.contains(_tNotifyData.m_wstrFunctionName))
	{
		m_umapFunctions[_tNotifyData.m_wstrFunctionName]();
	}
}
