#include "pch.h"
#include "Animation.h"
#include "Channel.h"
#include "Bone.h"
#include "Model.h"
#include "StringUtility.h"

Engine::CAnimation::CAnimation(const CAnimation& rhs)
	:m_fDuration(rhs.m_fDuration), m_fTickPerSec(rhs.m_fTickPerSec), m_fTrackPosition(rhs.m_fTrackPosition)
{
	m_wpOwnerModel = rhs.m_wpOwnerModel.lock();
	m_vecBonesRef = rhs.m_vecBonesRef;
	m_umapChannels = rhs.m_umapChannels;
	m_lstAnimNotifyData = rhs.m_lstAnimNotifyData;
}

void Engine::CAnimation::MoveTrackPosition(float _fTrackPos, const list<int32>& _lstBoneIndices)
{
	m_fTrackPosition = _fTrackPos;

	for (auto& BoneIndex : _lstBoneIndices)
	{
		if (m_umapChannels.contains(BoneIndex))
		{
			m_umapChannels[BoneIndex]->PlayKeyFrame(m_fTrackPosition, m_vecBonesRef, m_vRootMotionVelocity);
		}
	}
}

void Engine::CAnimation::SetLerpTime(float _fLerpTime, const list<int32>& _lstBoneIndices)
{
	for (auto& BoneIndex : _lstBoneIndices)
	{
		if (m_umapChannels.contains(BoneIndex))
		{
			m_umapChannels[BoneIndex]->SetLerpTime(_fLerpTime);
		}
	}
}

void Engine::CAnimation::ResetLerpChecker(const list<int32>& _lstBoneIndiecse)
{
	for (auto& iIndex : _lstBoneIndiecse)
	{
		if(m_umapChannels.contains(iIndex))
			m_umapChannels[iIndex]->ResetLerpChecker();
	}
}

list<FAnimNotifyData> Engine::CAnimation::GetCurrentFrameNotifyData()
{
	list<FAnimNotifyData> lstResult = m_lstActivatedNotifies;

	// 이번 프레임에 재생된 노티파이들을 담은 후 재생확인용 bool 을 true로 바꿔준다
	m_lstActivatedNotifies.clear();

	return lstResult;
}

bool Engine::CAnimation::IsSameGroup(shared_ptr<CAnimation> _spOtherAnimation)
{
	if (m_iAnimationGroupID == -1) { return false; }

	return m_iAnimationGroupID == _spOtherAnimation->GetGroupID();
}

shared_ptr<CAnimation> Engine::CAnimation::Create(ifstream& _ifFile, shared_ptr<CModel> _spModel)
{
	shared_ptr<CAnimation> spInstance = make_shared<CAnimation>();

	CHECKF(spInstance, L"CAnimation Create Failed");
	spInstance->Initialize(_ifFile, _spModel);

	return spInstance;
}

void Engine::CAnimation::Initialize(ifstream& _ifFile, shared_ptr<CModel> _spModel)
{
	m_wpOwnerModel = _spModel;

	m_vecBonesRef = m_wpOwnerModel.lock()->GetBones();

	int32 iAnimationnameLen = 0;
	wchar_t wszAnimationName[MAX_PATH] = {};
	_ifFile.read(reinterpret_cast<char*>(&iAnimationnameLen), sizeof(iAnimationnameLen));
	_ifFile.read(reinterpret_cast<char*>(wszAnimationName), iAnimationnameLen * sizeof(wchar_t));
	m_wstrAnimationName = wszAnimationName;

	_ifFile.read(reinterpret_cast<char*>(&m_fDuration), sizeof(m_fDuration));
	_ifFile.read(reinterpret_cast<char*>(&m_fTickPerSec), sizeof(m_fTickPerSec));

	int32 iNumChannel = 0;
	_ifFile.read(reinterpret_cast<char*>(&iNumChannel), sizeof(iNumChannel));
	for (size_t i = 0; i < iNumChannel; ++i)
	{
		int32 iBoneIndex = 0;
		_ifFile.read(reinterpret_cast<char*>(&iBoneIndex), sizeof(iBoneIndex));
		m_umapChannels.emplace(iBoneIndex, CChannel::Create(_ifFile, _spModel));
	}

	InitAnimNotifyData();

	FindRootBoneIndex();
}

void Engine::CAnimation::Release()
{
	for (auto& spBone : m_vecBonesRef)
	{
		spBone->Release();
		spBone = nullptr;
	}
	m_vecBonesRef.clear();

	for (auto& pair : m_umapChannels)
	{
		pair.second->Release();
		pair.second = nullptr;
	}
	m_umapChannels.clear();

	m_lstAnimNotifyData.clear();
}

void Engine::CAnimation::InitAnimNotifyData()
{
	wstring wstrAnimName = m_wstrAnimationName;

	int32 iIndex = (int32)wstrAnimName.find('_');
	wstring wstrAMFilePath = L"../../Resource/Models/AnimNotify/AM_" + wstrAnimName.substr(iIndex+1) + L".json";

	ifstream animNotify(wstrAMFilePath);

	if (animNotify.fail())
		return;

	Json::Value notify;
	animNotify >> notify;

	for (auto jsonIter = notify.begin(); jsonIter != notify.end(); ++jsonIter)
	{
		string strType = (*jsonIter)["Type"].asString();

		// 이벤트 시작 프레임 / 끝 프레임
		float fStartTime = (*jsonIter)["StartTime"].asFloat();
		float fEndTime = (*jsonIter)["EndTime"].asFloat();

		fStartTime *= m_fTickPerSec;
		fEndTime *= m_fTickPerSec;

		FAnimNotifyData tNotify;
		tNotify.m_fStartTime = fStartTime;
		tNotify.m_fEndTime = fEndTime;
		tNotify.m_fDuration = (*jsonIter)["Duration"].asFloat();

		// 타입별 데이터 생성 후 삽입
		if (strType == "Collision")
		{
			tNotify.m_eNotifyType = ENotifyType::COLLISION;
			tNotify.m_tCollisionNotify.m_iAttachBoneIndex = m_wpOwnerModel.lock()->GetBoneIndex(CStringUtility::ToWideString((*jsonIter)["AttachBoneName"].asString()));

			Json::Value offsetDatas = (*jsonIter)["OffsetDataArray"];
			int iCnt = 0;
			for (auto offsetData = offsetDatas.begin(); offsetData != offsetDatas.end(); ++offsetData)
			{
				// 충돌체 오프셋
				tNotify.m_tCollisionNotify.m_vecColliderRadius.push_back((*offsetData)["SphereRadius"].asFloat() * 0.01f);
				tNotify.m_tCollisionNotify.m_vecColliderRelativeLocation.push_back(Vector3((*offsetData)["vRelativeLocationX"].asFloat() * 0.01f, -(*offsetData)["vRelativeLocationY"].asFloat() * 0.01f, -(*offsetData)["vRelativeLocationZ"].asFloat() * 0.01f));

				++iCnt;
			}

			tNotify.m_tCollisionNotify.m_iColliderCount = iCnt;
		}
		else if (strType == "Sound")
		{
			tNotify.m_eNotifyType = ENotifyType::SOUND;

			tNotify.m_tSoundNotify.m_wstrSoundID = CStringUtility::ToWideString((*jsonIter)["SoundID"].asString());
		}
		else if (strType == "Effect")
		{
			tNotify.m_eNotifyType = ENotifyType::EFFECT;

			tNotify.m_tEffectNotify.m_iAttachBoneIndex = m_wpOwnerModel.lock()->GetBoneIndex(CStringUtility::ToWideString((*jsonIter)["AttachBoneName"].asString()));
			tNotify.m_tEffectNotify.m_wstrEffectName = CStringUtility::ToWideString((*jsonIter)["EffectName"].asString());
			tNotify.m_tEffectNotify.m_bUseFollowBone = (*jsonIter)["UseFollowBone"].asBool();
			tNotify.m_tEffectNotify.m_vRelativeLocation = Vector3((*jsonIter)["vRelativeLocationX"].asFloat() * 0.01f, (*jsonIter)["vRelativeLocationY"].asFloat() * 0.01f, (*jsonIter)["vRelativeLocationZ"].asFloat() * 0.01f);
		}
		else if (strType == "Function")
		{
			tNotify.m_eNotifyType = ENotifyType::FUNC;

			tNotify.m_wstrFunctionName = CStringUtility::ToWideString((*jsonIter)["FunctionName"].asString());
		}

		m_lstAnimNotifyData.push_back({ tNotify, false });
	}
}

void Engine::CAnimation::FindRootBoneIndex()
{
	for (auto& spBone : m_vecBonesRef)
	{
		if (spBone->GetBoneName() == L"root")
		{
			m_iRootChannelIndex = spBone->GetMyIndex();
			return;
		}
	}
}

void Engine::CAnimation::ResetChannel()
{
	ResetNotifyActivated();

	m_fTrackPosition = 0.f;
	m_bFinish = false;
}

bool Engine::CAnimation::PlayAnimation(float _fDeltaSeconds, bool _bUseReversePlay, const list<int32>& _lstBoneIndices, float _fLoopStartRatio)
{
	bool bResult = false;

	// 역재생 옵션에 따른 재생방식 변경
	if (_bUseReversePlay)
	{
		m_fTrackPosition -= m_fTickPerSec * _fDeltaSeconds;

		if (!m_bFinish && GetCurrentFrameRatio() < 0.15f && _fDeltaSeconds != 0.f)
		{
			m_bFinish = true;
			bResult = true;
		}
		
		if (m_fTrackPosition < m_fDuration * _fLoopStartRatio)
		{
			m_fTrackPosition = m_fDuration;
			m_bFinish = false;

			m_umapChannels[m_iRootChannelIndex]->SetPrevTranslationToEnd();
			ResetNotifyActivated();
		}
	}
	else
	{
		m_fTrackPosition += m_fTickPerSec * _fDeltaSeconds;

		if (!m_bFinish && GetCurrentFrameRatio() > 0.85f && _fDeltaSeconds != 0.f)
		{
			m_bFinish = true;
			bResult = true;
		}

		if (m_fTrackPosition > m_fDuration)
		{
			m_fTrackPosition = m_fDuration * _fLoopStartRatio;
			m_bFinish = false;

			m_umapChannels[m_iRootChannelIndex]->SetPrevTranslationToStart();
			ResetNotifyActivated();
		}
	}

 	for (auto& BoneIndex : _lstBoneIndices)
	{
		if (m_umapChannels.contains(BoneIndex))
		{
			m_umapChannels[BoneIndex]->PlayKeyFrame(m_fTrackPosition, m_vecBonesRef, m_vRootMotionVelocity);
		}
	}

	// 노티파이 실행
	if(_fDeltaSeconds != 0.f && !m_wpOwnerModel.lock()->IsDownLerping() && !m_wpOwnerModel.lock()->IsUpperLerping())
		PlayNotify(_bUseReversePlay);

	return bResult;
}

void Engine::CAnimation::PlayAnimationInLerp(float _fDeltaSeconds, bool _bUseReversePlay, const list<int32>& _lstBoneIndices)
{
	// 역재생 옵션에 따른 재생방식 변경
	if (_bUseReversePlay)
	{
		m_fTrackPosition -= m_fTickPerSec * _fDeltaSeconds;

		if (m_fTrackPosition <= 0.f)
		{
			m_fTrackPosition = 0.f;
			return;
		}
	}
	else
	{
		m_fTrackPosition += m_fTickPerSec * _fDeltaSeconds;

		if (m_fTrackPosition >= m_fDuration)
		{
			m_fTrackPosition = m_fDuration;
			return;
		}
	}

	for (auto& BoneIndex : _lstBoneIndices)
	{
		if (m_umapChannels.contains(BoneIndex))
		{
			m_umapChannels[BoneIndex]->PlayKeyFrame(m_fTrackPosition, m_vecBonesRef, m_vRootMotionVelocity);
		}
	}
}

bool Engine::CAnimation::ChangeAnimationLerp(float _fDeltaSeconds, const shared_ptr<CAnimation> _spNextAnim, bool _bUseReversePlay, const list<int32>& _lstBoneIndices)
{
	bool bResult = true;

	auto nextChannels = _spNextAnim->GetChannels();

	for (auto& BoneIndex : _lstBoneIndices)
	{
		bool nextChannelContains = nextChannels.contains(BoneIndex);
		bool currentChannelContains = m_umapChannels.contains(BoneIndex);

		// 다음 채널도 해당하는 뼈와 매칭되는 경우
		if (currentChannelContains && nextChannelContains)
		{
			bResult = m_umapChannels[BoneIndex]->LerpToAnotherAnimKeyframe(_fDeltaSeconds, nextChannels[BoneIndex]->GetCurrentKeyFrame(), m_vecBonesRef);
		}
		// 현재 채널만 뼈와 매칭되는 경우
		else if (currentChannelContains)
		{
			bResult = m_umapChannels[BoneIndex]->LerpToStartKeyFrame(_fDeltaSeconds, m_vecBonesRef);
		}
	}

	return bResult;
}

void Engine::CAnimation::ResetNotifyActivated()
{
	for (auto& pair : m_lstAnimNotifyData)
	{
		pair.second = false;
	}
}

void Engine::CAnimation::PlayNotify(bool _bPlayReverse)
{
	for (auto& pair : m_lstAnimNotifyData)
	{
		if (!pair.second && pair.first.m_fStartTime < m_fTrackPosition)
		{
			m_lstActivatedNotifies.push_back(pair.first);
			pair.second = true;
		}
	}
}
