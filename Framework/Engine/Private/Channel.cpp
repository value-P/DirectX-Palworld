#include "pch.h"
#include "Channel.h"
#include "Bone.h"

std::shared_ptr<CChannel> Engine::CChannel::Create(ifstream& _ifstream, shared_ptr<CModel> _spModel)
{
	shared_ptr<CChannel> pInstance = make_shared<CChannel>();

	CHECKF(pInstance != nullptr, L"CChannel Create Failed");

	pInstance->Initialize(_ifstream, _spModel);
	return pInstance;
}

void Engine::CChannel::Initialize(ifstream& _ifFile, shared_ptr<CModel> _spModel)
{
	int32 iChannelNameLen = 0;
	wchar_t wszChannelName[MAX_PATH] = {};
	_ifFile.read(reinterpret_cast<char*>(&iChannelNameLen), sizeof(iChannelNameLen));
	_ifFile.read(reinterpret_cast<char*>(wszChannelName), iChannelNameLen * sizeof(wchar_t));
	m_wstrChannelName = wszChannelName;

	_ifFile.read(reinterpret_cast<char*>(&m_iBoneIndex), sizeof(m_iBoneIndex));

	_ifFile.read(reinterpret_cast<char*>(&m_iNumKeyFrames), sizeof(m_iNumKeyFrames));
	m_vecKeyFrames.resize(m_iNumKeyFrames);
	for (size_t i = 0; i < m_iNumKeyFrames; ++i)
	{
		_ifFile.read(reinterpret_cast<char*>(&m_vecKeyFrames[i].fTime), sizeof(m_vecKeyFrames[i].fTime));
		_ifFile.read(reinterpret_cast<char*>(&m_vecKeyFrames[i].vPosition.x), sizeof(m_vecKeyFrames[i].vPosition.x));
		_ifFile.read(reinterpret_cast<char*>(&m_vecKeyFrames[i].vPosition.y), sizeof(m_vecKeyFrames[i].vPosition.y));
		_ifFile.read(reinterpret_cast<char*>(&m_vecKeyFrames[i].vPosition.z), sizeof(m_vecKeyFrames[i].vPosition.z));
		_ifFile.read(reinterpret_cast<char*>(&m_vecKeyFrames[i].qRotation.x), sizeof(m_vecKeyFrames[i].qRotation.x));
		_ifFile.read(reinterpret_cast<char*>(&m_vecKeyFrames[i].qRotation.y), sizeof(m_vecKeyFrames[i].qRotation.y));
		_ifFile.read(reinterpret_cast<char*>(&m_vecKeyFrames[i].qRotation.z), sizeof(m_vecKeyFrames[i].qRotation.z));
		_ifFile.read(reinterpret_cast<char*>(&m_vecKeyFrames[i].qRotation.w), sizeof(m_vecKeyFrames[i].qRotation.w));
		_ifFile.read(reinterpret_cast<char*>(&m_vecKeyFrames[i].vScale.x), sizeof(m_vecKeyFrames[i].vScale.x));
		_ifFile.read(reinterpret_cast<char*>(&m_vecKeyFrames[i].vScale.y), sizeof(m_vecKeyFrames[i].vScale.y));
		_ifFile.read(reinterpret_cast<char*>(&m_vecKeyFrames[i].vScale.z), sizeof(m_vecKeyFrames[i].vScale.z));
	}

	m_tCurrentKeyFrame = m_vecKeyFrames[0];
}

void Engine::CChannel::Release()
{
	m_vecKeyFrames.clear();
	m_vecKeyFrames.shrink_to_fit();
}

void Engine::CChannel::PlayKeyFrame(float _fTrackPosition, const vector<std::shared_ptr<CBone>>& _vecBones, Vector3& _vRootMotionVelocity)
{
	if (_fTrackPosition >= m_vecKeyFrames[m_iNumKeyFrames - 1].fTime)
	{
		m_tCurrentKeyFrame.vScale = m_vecKeyFrames.back().vScale;
		m_tCurrentKeyFrame.qRotation = m_vecKeyFrames.back().qRotation;
		m_tCurrentKeyFrame.vPosition = m_vecKeyFrames.back().vPosition;
	}
	else
	{
		int iIndex = 0;
		while (true)
		{
			if (m_vecKeyFrames[iIndex].fTime <= _fTrackPosition && m_vecKeyFrames[iIndex + 1].fTime > _fTrackPosition)
				break;

			++iIndex;
		}

		KEYFRAME tCurrentKeyFrame = m_vecKeyFrames[iIndex];
		KEYFRAME tNextKeyFrame = m_vecKeyFrames[iIndex + 1];

		float fRatio = (_fTrackPosition - tCurrentKeyFrame.fTime) / (tNextKeyFrame.fTime - tCurrentKeyFrame.fTime);

		m_tCurrentKeyFrame.vScale	 = Vector3::Lerp(tCurrentKeyFrame.vScale, tNextKeyFrame.vScale, fRatio);
		m_tCurrentKeyFrame.qRotation = Quaternion::Slerp(tCurrentKeyFrame.qRotation, tNextKeyFrame.qRotation, fRatio);
		m_tCurrentKeyFrame.vPosition = Vector3::Lerp(tCurrentKeyFrame.vPosition, tNextKeyFrame.vPosition, fRatio);
	}

	if (m_wstrChannelName == L"root")
	{
		_vRootMotionVelocity = m_tCurrentKeyFrame.vPosition - m_vPrevTranslation;
		m_vPrevTranslation = m_tCurrentKeyFrame.vPosition;
		m_tCurrentKeyFrame.vPosition = Vector3(0.f,0.f,0.f);
	}

	Vector4 vScale				= XMLoadFloat3(&m_tCurrentKeyFrame.vScale);
	Vector4 vTranslation		= XMVectorSetW(m_tCurrentKeyFrame.vPosition,1.f);
	Matrix matTransformation	= XMMatrixAffineTransformation(vScale, Vector4(0.f, 0.f, 0.f, 1.f), m_tCurrentKeyFrame.qRotation, vTranslation);

	_vecBones[m_iBoneIndex]->SetTransformationMatrix(matTransformation);
}

bool Engine::CChannel::LerpToAnotherAnimKeyframe(float _fAddTime, KEYFRAME _tNextKeyFrame, const vector<std::shared_ptr<CBone>>& _vecBones)
{
	m_fLerpChecker += _fAddTime;

	float fRatio = 0.f;
	
	fRatio = m_fLerpChecker / m_fLerpTime;

	// 보간 끝
	if (fRatio >= 1.f)
		fRatio = 1.f;

	m_tCurrentKeyFrame.vScale = Vector3::Lerp(m_tCurrentKeyFrame.vScale, _tNextKeyFrame.vScale, fRatio);
	m_tCurrentKeyFrame.vPosition = Vector3::Lerp(m_tCurrentKeyFrame.vPosition, _tNextKeyFrame.vPosition, fRatio);
	m_tCurrentKeyFrame.qRotation = Quaternion::Slerp(m_tCurrentKeyFrame.qRotation, _tNextKeyFrame.qRotation, fRatio);

	Vector4 vScale = XMLoadFloat3(&m_tCurrentKeyFrame.vScale);
	Vector4 vTranslation = XMVectorSetW(m_tCurrentKeyFrame.vPosition, 1.f);

	Matrix matTransformation = XMMatrixAffineTransformation(vScale, Vector4(0.f, 0.f, 0.f, 1.f), m_tCurrentKeyFrame.qRotation, vTranslation);

	_vecBones[m_iBoneIndex]->SetTransformationMatrix(matTransformation);

	if (fRatio >= 1.f)
	{
		m_fLerpChecker = 0.f;
		m_fLerpTime = m_fDefaultLerpTime;
		return true;
	}

	return false;
}

bool Engine::CChannel::LerpToStartKeyFrame(float _fAddTime, const vector<std::shared_ptr<CBone>>& _vecBones)
{
	m_fLerpChecker += _fAddTime;

	float fRatio = 0.f;

	fRatio = m_fLerpChecker / m_fLerpTime;

	// 보간 끝
	if (fRatio >= 1.f)
	{
		m_fLerpChecker = 0.f;
		m_fLerpTime = m_fDefaultLerpTime;
		return true;
	}

	KEYFRAME tStartKeyFrame = m_vecKeyFrames.front();

	m_tCurrentKeyFrame.vScale = Vector3::Lerp(m_tCurrentKeyFrame.vScale, tStartKeyFrame.vScale, fRatio);
	m_tCurrentKeyFrame.vPosition = Vector3::Lerp(m_tCurrentKeyFrame.vPosition, tStartKeyFrame.vPosition, fRatio);
	m_tCurrentKeyFrame.qRotation = Quaternion::Slerp(m_tCurrentKeyFrame.qRotation, tStartKeyFrame.qRotation, fRatio);

	Vector4 vScale = XMLoadFloat3(&m_tCurrentKeyFrame.vScale);
	Vector4 vTranslation = XMVectorSetW(m_tCurrentKeyFrame.vPosition,1.f);

	Matrix matTransformation = XMMatrixAffineTransformation(vScale, Vector4(0.f, 0.f, 0.f, 1.f), m_tCurrentKeyFrame.qRotation, vTranslation);

	_vecBones[m_iBoneIndex]->SetTransformationMatrix(matTransformation);

	return false;
}