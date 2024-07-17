#include "pch.h"
#include "Model.h"

#include "MeshHandler.h"

#include "Bone.h"
#include "Animation.h"
#include "Material.h"
#include "MeshBuffer.h"
#include "SphereComponent.h"
#include "Component.h"
#include "StringUtility.h"
#include "EffectShader.h"
#include "EngineModule.h"
#include "DeviceManager.h"

shared_ptr<Engine::CModel::FColliderInfo> Engine::CModel::GetColliderInfo(wstring _wstrColliderName)
{
	for (auto& tColliderInfo : m_vecColliderInfos)
	{
		if (tColliderInfo->m_wstrColliderName == _wstrColliderName)
		{
			return tColliderInfo;
		}
	}
	return nullptr;
}

int32 Engine::CModel::GetBoneIndex(const wstring& _wstrBoneName)
{
	int32 iBoneIndex = -1; // 부모가 없는 경우 -1

	auto iter = find_if(m_vecBones.begin(), m_vecBones.end(), [&](shared_ptr<CBone> _spBone) {
		iBoneIndex++;
		return _wstrBoneName == _spBone->GetBoneName();
		});

	if (iter == m_vecBones.end()) { return -1; }

	return iBoneIndex;
}

float Engine::CModel::GetCurrentUpperFrameRatio()
{
	return m_umapAnimations[m_tCurrentUpperAnimDat.m_iAnimID]->GetCurrentFrameRatio();
}

float Engine::CModel::GetCurrentDownFrameRatio()
{
	return m_umapAnimations[m_tCurrentDownAnimDat.m_iAnimID]->GetCurrentFrameRatio();
}

void Engine::CModel::SetRotationExceptBone(const wstring& _wstrBoneName)
{
	m_iExceptRotationBoneIndex = GetBoneIndex(_wstrBoneName);
}

list<FAnimNotifyData> Engine::CModel::GetCurrentUpperAnimNotifyData()
{
	if (!m_umapAnimations.contains(m_tCurrentUpperAnimDat.m_iAnimID) || m_tCurrentUpperAnimDat.m_iAnimID == m_tCurrentDownAnimDat.m_iAnimID) return list<FAnimNotifyData>();

	return m_umapAnimations[m_tCurrentUpperAnimDat.m_iAnimID]->GetCurrentFrameNotifyData();
}

list<FAnimNotifyData> Engine::CModel::GetCurrentDownAnimNotifyData()
{
	if (!m_umapAnimations.contains(m_tCurrentDownAnimDat.m_iAnimID)) return list<FAnimNotifyData>();

	return m_umapAnimations[m_tCurrentDownAnimDat.m_iAnimID]->GetCurrentFrameNotifyData();
}

shared_ptr<CBone> Engine::CModel::GetBonePtr(const wstring& _wstrBoneName)
{
	int32 iBoneIndex = GetBoneIndex(_wstrBoneName);

	if (iBoneIndex == -1) return nullptr;

	return m_vecBones[iBoneIndex];
}

shared_ptr<CBone> Engine::CModel::GetBonePtr(int32 _iBoneIndex)
{
	if (m_vecBones.size() <= _iBoneIndex || _iBoneIndex < 0)
		return nullptr;

	return m_vecBones[_iBoneIndex];
}

void Engine::CModel::SetUseAnimation(bool _bUseAnimation, bool _bUseIdentityMatrices)
{
	if (m_bUseAnimation == _bUseAnimation) { return; }

	if (!_bUseAnimation)
	{
		for (auto& spBone : m_vecBones)
		{
			spBone->SetMatrixToInitial();
		}

		InvalidateCombinedMatrix(m_vecBones[0], 0.f);
	}

	m_bUseAnimation = _bUseAnimation;
	m_bUseIdentityMatrices = _bUseIdentityMatrices;
}

FModelAnimData Engine::CModel::GetModelAnimationData()
{
	FModelAnimData tAnimationData;

	int32 iDownAnimID = m_tCurrentDownAnimDat.m_iAnimID;
	tAnimationData.m_vRootmotionVelocity = m_umapAnimations[iDownAnimID]->GetRootMotionVelocity();

	tAnimationData.m_tBaseAnimData.m_wstrCurrentAnimName = m_umapAnimations[iDownAnimID]->GetAnimationName();
	tAnimationData.m_tBaseAnimData.m_iCurrentAnimID = iDownAnimID;
	tAnimationData.m_tBaseAnimData.m_iPrevAnimID = m_tPrevDownAnimDat.m_iAnimID;
	tAnimationData.m_tBaseAnimData.m_iNextAnimID = m_tNextDownAnimDat.m_iAnimID;
	tAnimationData.m_tBaseAnimData.m_bIsLerping = m_bDownAnimChangeFlag;
	tAnimationData.m_tBaseAnimData.m_fCurrentKeyFrameRate = m_umapAnimations[iDownAnimID]->GetCurrentFrameRatio();
	tAnimationData.m_tBaseAnimData.m_fDuration = m_umapAnimations[iDownAnimID]->GetDuration();
	tAnimationData.m_tBaseAnimData.m_fTrackPosition = m_umapAnimations[iDownAnimID]->GetTrackPosition();
	tAnimationData.m_tBaseAnimData.m_iLoopCount = m_iNumDownCurrentAnimLoop;
	tAnimationData.m_tBaseAnimData.m_bLoopEnd = m_bDownLoopEnd; // 현재 루프카운트와 이전 루프카운트가 다른 시점이 애니메이션이 끝난시점

	int32 iUpperAnimID = m_tCurrentUpperAnimDat.m_iAnimID;
	tAnimationData.m_tBlendAnimData.m_wstrCurrentAnimName = m_umapAnimations[iUpperAnimID]->GetAnimationName();
	tAnimationData.m_tBlendAnimData.m_iCurrentAnimID = iUpperAnimID;
	tAnimationData.m_tBlendAnimData.m_iPrevAnimID = m_tPrevUpperAnimDat.m_iAnimID;
	tAnimationData.m_tBlendAnimData.m_iNextAnimID = m_tNextUpperAnimDat.m_iAnimID;
	tAnimationData.m_tBlendAnimData.m_bIsLerping = m_bUpperAnimChangeFlag;
	tAnimationData.m_tBlendAnimData.m_fCurrentKeyFrameRate = m_umapAnimations[iUpperAnimID]->GetCurrentFrameRatio();
	tAnimationData.m_tBlendAnimData.m_fDuration = m_umapAnimations[iUpperAnimID]->GetDuration();
	tAnimationData.m_tBlendAnimData.m_fTrackPosition = m_umapAnimations[iUpperAnimID]->GetTrackPosition();
	tAnimationData.m_tBlendAnimData.m_iLoopCount = m_iNumUpperCurrentAnimLoop;
	tAnimationData.m_tBlendAnimData.m_bLoopEnd = m_bUpperLoopEnd; // 현재 루프카운트와 이전 루프카운트가 다른 시점이 애니메이션이 끝난시점

	return tAnimationData;
}

void Engine::CModel::SetTrackPosition(float _fTrackPosition)
{
	m_umapAnimations[m_tCurrentDownAnimDat.m_iAnimID]->MoveTrackPosition(_fTrackPosition, m_lstDownBoneIndices);
	m_umapAnimations[m_tCurrentUpperAnimDat.m_iAnimID]->MoveTrackPosition(_fTrackPosition, m_lstUpperBoneIndices);

	InvalidateCombinedMatrix(m_vecBones[0], 0.f);
}

vector<wstring> Engine::CModel::GetAnimationNames()
{
	vector<wstring> vecAnimationNames;
	vecAnimationNames.reserve(m_umapAnimations.size());

	for (auto pair : m_umapAnimations)
	{
		vecAnimationNames.push_back(pair.second->GetAnimationName());
	}

	return vecAnimationNames;
}

wstring Engine::CModel::GetAnimationName(int32 _iAnimID)
{
	if (!m_umapAnimations.contains(_iAnimID)) { return L""; }

	return m_umapAnimations[_iAnimID]->GetAnimationName();
}

shared_ptr<CModel> Engine::CModel::Create(const wstring& _wstrDatPathFile, Matrix _matPivot, EMeshManageType _eMeshManageType, const shared_ptr<CModel> _spParentModel, const wstring& _wstrUpperBoneName, const wstring& _wstrAnimGroupJsonPath)
{
	shared_ptr<CModel> spModel = make_shared<CModel>();
	ENSUREF(spModel->Initialize(_wstrDatPathFile, _matPivot, _eMeshManageType, _spParentModel, _wstrUpperBoneName, _wstrAnimGroupJsonPath), L"Failed to initialize model.");
	return spModel;
}

HRESULT Engine::CModel::Initialize(const wstring& _wstrDatPathFile, Matrix _matPivot, EMeshManageType _eMeshManageType, const shared_ptr<CModel> _spParentModel, const wstring& _wstrUpperBoneName, const wstring& _wstrAnimGroupJsonPath)
{	
	// 메쉬 관리법 저장
	m_eMeshType = _eMeshManageType;

	// 피벗 매트릭스 저장
	m_matPivot = _matPivot;

	// 모델 이름 저장
	{
		filesystem::path filePath(_wstrDatPathFile);
		m_wstrModelName = filePath.filename().wstring();
		size_t iPoint = m_wstrModelName.find('.');
		m_wstrModelName = m_wstrModelName.substr(0, iPoint);
	}

	// 모델 데이터 읽어오기
	ifstream ifFile(_wstrDatPathFile, ios::binary);

	ifFile.read(reinterpret_cast<char*>(&m_eModelType), sizeof(m_eModelType));
	if (m_eModelType == EModelType::ANIM)
	{
		m_matPivot = Matrix::Identity;
	}

	int32 iNumBones = 0;
	ifFile.read(reinterpret_cast<char*>(&iNumBones), sizeof(iNumBones));
	for (size_t i = 0; i < iNumBones; ++i)
	{
		m_vecBones.push_back(CBone::Create(ifFile, _matPivot));
	}

	int32 iNumAnimations = 0;
	ifFile.read(reinterpret_cast<char*>(&iNumAnimations), sizeof(iNumAnimations));
	for (size_t i = 0; i < iNumAnimations; ++i)
	{
		int32 iAnimationNameLen = 0;
		wchar_t wszAnimationName[MAX_PATH] = {};
		ifFile.read(reinterpret_cast<char*>(&iAnimationNameLen), sizeof(iAnimationNameLen));
		ifFile.read(reinterpret_cast<char*>(wszAnimationName), iAnimationNameLen * sizeof(wchar_t));

		wstring AnimationName = wszAnimationName;
		int32 iID = findAnimationID(AnimationName);

		if (iID != -1)
		{
			m_umapAnimations.emplace(iID, CAnimation::Create(ifFile, shared_from_this()));
			m_umapAnimations[iID]->SetAnimationID(iID);
		}
	}

	int32 iNumObjects = 0;
	ifFile.read(reinterpret_cast<char*>(&iNumObjects), sizeof(iNumObjects));
	for (int32 i = 0; i < iNumObjects; ++i)
	{
		int32 iObjectNameLen = 0;
		wchar_t wszObjectName[MAX_PATH] = {};
		ifFile.read(reinterpret_cast<char*>(&iObjectNameLen), sizeof(iObjectNameLen));
		ifFile.read(reinterpret_cast<char*>(const_cast<wchar_t*>(wszObjectName)), iObjectNameLen * sizeof(wchar_t));

		int32 iNumObjectMeshes = 0;
		ifFile.read(reinterpret_cast<char*>(&iNumObjectMeshes), sizeof(iNumObjectMeshes));

		vector<wstring> vecMeshNames;
		for (int32 i = 0; i < iNumObjectMeshes; ++i)
		{
			int32 iMeshNameLen = 0;
			wchar_t wszMeshName[MAX_PATH] = {};
			ifFile.read(reinterpret_cast<char*>(&iMeshNameLen), sizeof(iMeshNameLen));
			ifFile.read(reinterpret_cast<char*>(wszMeshName), iMeshNameLen * sizeof(wchar_t));

			vecMeshNames.push_back(wszMeshName);
		}

		Matrix matTransform;
		for (int32 i = 0; i < 4; ++i)
		{
			for (int32 j = 0; j < 4; ++j)
			{
				ifFile.read(reinterpret_cast<char*>(&matTransform.m[i][j]), sizeof(matTransform.m[i][j]));
			}
		}

		matTransform = matTransform * m_matPivot;

		if (m_eModelType == EModelType::MAP_OBJECT)
		{
			Vector3 vWorldPosition = matTransform.Translation();

			// 옥트리

		}

		for (int32 i = 0; i < iNumObjectMeshes; ++i)
		{
			ifstream ifMeshDatFile(L"../../Resource/Models/Meshes/" + vecMeshNames[i] + L".dat", std::ios::binary);
			shared_ptr<CMeshBuffer> spMeshBuffer;

			switch (_eMeshManageType)
			{
			case Engine::EMeshManageType::UNIQUE:
				spMeshBuffer = CMeshBuffer::Create(_wstrDatPathFile, ifMeshDatFile, m_matPivot, m_eModelType, _eMeshManageType);
				spMeshBuffer->SetMeshName(vecMeshNames[i]);
				break;
			case Engine::EMeshManageType::INSTANCE:
				CMeshHandler::GetInstance()->RequestMeshBuffer(_wstrDatPathFile, ifMeshDatFile, &spMeshBuffer, vecMeshNames[i], m_matPivot, m_eModelType, _eMeshManageType);
				break;
			}

			FMeshInfo tMeshInfo;
			tMeshInfo.m_matTransform = matTransform;
			tMeshInfo.m_spMeshBuffer = spMeshBuffer;

			EShaderType eShaderType = spMeshBuffer->GetMaterial()->GetShaderType();
			m_umapMeshInfos[eShaderType].push_back(make_shared<FMeshInfo>(tMeshInfo));
			ifMeshDatFile.close();
		}
	}

	if (m_umapMeshInfos.contains(EShaderType::ARCHITECTURE))
	{
		float fMin = FLT_MAX;
		float fMax = -FLT_MAX;

		for (auto& ShaderTypePair : m_umapMeshInfos)
		{
			for (auto& tMeshInfo : ShaderTypePair.second)
			{
				auto& vecPositons = tMeshInfo->m_spMeshBuffer->GetVertexPositions();
				for (auto& vPosition : vecPositons)
				{
					Vector4 vTransformed = Vector4::Transform(Vector4(vPosition.x, vPosition.y, vPosition.z, 1.f), tMeshInfo->m_matTransform);
					if (vTransformed.y < fMin)
					{
						fMin = vTransformed.y;
					}
					if (vTransformed.y > fMax)
					{
						fMax = vTransformed.y;
					}
				}
			}
		}

		float fExtendY = fabs((fMax - fMin)) * 0.5f;

		for (auto& ShaderTypePair : m_umapMeshInfos)
		{
			for (auto& tMeshInfo : ShaderTypePair.second)
			{
				tMeshInfo->m_spMeshBuffer->GetMaterial()->SetFloat(L"ExtendY", fExtendY);
				tMeshInfo->m_spMeshBuffer->GetMaterial()->SetVector3(L"Center", Vector3(0, fMin + fExtendY, 0));
			}
		}
	}

	int32 iNumColliders = 0;
	ifFile.read(reinterpret_cast<char*>(&iNumColliders), sizeof(iNumColliders));
	for (int32 i = 0; i < iNumColliders; ++i)
	{
		FColliderInfo tCollider;

		ifFile.read(reinterpret_cast<char*>(&tCollider.m_eColliderType), sizeof(tCollider.m_eColliderType));

		int32 iColliderNameLen = 0;
		wchar_t wszColliderName[MAX_PATH] = {};
		ifFile.read(reinterpret_cast<char*>(&iColliderNameLen), sizeof(iColliderNameLen));
		ifFile.read(reinterpret_cast<char*>(const_cast<wchar_t*>(wszColliderName)), iColliderNameLen * sizeof(wchar_t));
		tCollider.m_wstrColliderName = wszColliderName;

		int32 iParentBoneNameLen = 0;
		wchar_t wszParentBoneName[MAX_PATH] = {};
		ifFile.read(reinterpret_cast<char*>(&iParentBoneNameLen), sizeof(iParentBoneNameLen));
		ifFile.read(reinterpret_cast<char*>(const_cast<wchar_t*>(wszParentBoneName)), iParentBoneNameLen * sizeof(wchar_t));
		tCollider.m_wstrParentBoneName = wszParentBoneName;

		ifFile.read(reinterpret_cast<char*>(&tCollider.m_vCenter.x), sizeof(tCollider.m_vCenter.x));
		ifFile.read(reinterpret_cast<char*>(&tCollider.m_vCenter.y), sizeof(tCollider.m_vCenter.y));
		ifFile.read(reinterpret_cast<char*>(&tCollider.m_vCenter.z), sizeof(tCollider.m_vCenter.z));

		ifFile.read(reinterpret_cast<char*>(&tCollider.m_fRadius), sizeof(tCollider.m_fRadius));

		if (tCollider.m_eColliderType == EColliderType::CAPSULE)
		{
			ifFile.read(reinterpret_cast<char*>(&tCollider.m_vRotationYPR.x), sizeof(tCollider.m_vRotationYPR.x));
			ifFile.read(reinterpret_cast<char*>(&tCollider.m_vRotationYPR.y), sizeof(tCollider.m_vRotationYPR.y));
			ifFile.read(reinterpret_cast<char*>(&tCollider.m_vRotationYPR.z), sizeof(tCollider.m_vRotationYPR.z));

			ifFile.read(reinterpret_cast<char*>(&tCollider.m_fLength), sizeof(tCollider.m_fLength));
		}

		m_vecColliderInfos.push_back(make_shared<FColliderInfo>(tCollider));
	}

	ifFile.close();

	if (_spParentModel)
	{
		ExChangeBonesWithParent(_spParentModel);
		m_bUseRefBones = true;
	}
	else
	{
		if (m_eModelType == EModelType::ANIM)
		{
			InitBoneGroup(_wstrUpperBoneName);
			InitAnimGroup(_wstrAnimGroupJsonPath);

			if (m_umapAnimations.empty()) { return S_OK; }
			ChangeAnimation(m_umapAnimations.begin()->first, false, 0.f);
		}
	}

	return S_OK;
}

void Engine::CModel::Release()
{
	for (auto& spBone : m_vecBones)
	{
		spBone->Release();
		spBone = nullptr;
	}
	m_vecBones.clear();

	for (auto& Pair : m_umapAnimations)
	{
		Pair.second->Release();
		Pair.second = nullptr;
	}
	m_umapAnimations.clear();

	for (auto& Pair : m_umapMeshInfos)
	{
		for (auto& tMeshInfo : Pair.second)
		{
			tMeshInfo->m_spMeshBuffer = nullptr;
		}
		Pair.second.clear();
	}
	m_umapMeshInfos.clear();
}

void Engine::CModel::InitBoneGroup(const wstring& _wstrUpperBoneName)
{
	m_iSpineBoneIndex = GetBoneIndex(_wstrUpperBoneName);

	SetBoneGroup(m_vecBones[0], m_lstDownBoneIndices);

	if (m_iSpineBoneIndex == -1) { return; }

	SetBoneGroup(m_vecBones[m_iSpineBoneIndex], m_lstUpperBoneIndices);

	for (auto downBoneIter = m_lstDownBoneIndices.begin(); downBoneIter != m_lstDownBoneIndices.end();)
	{
		bool bMatched = false;

		for (auto& upperBone : m_lstUpperBoneIndices)
		{
			if (*downBoneIter == upperBone)
			{
				bMatched = true;
				break;
			}
		}

		if (bMatched)
		{
			downBoneIter = m_lstDownBoneIndices.erase(downBoneIter);
		}
		else
		{
			++downBoneIter;
		}
	}
}

void Engine::CModel::InitAnimGroup(const wstring& _wstrAnimGroupJsonPath)
{
	ifstream ifAnimGroup(_wstrAnimGroupJsonPath);

	if (ifAnimGroup.fail()) { return; }

	Json::Value GroupJson;
	ifAnimGroup >> GroupJson;

	int32 iCnt = 0;
	for (auto GroupIter = GroupJson.begin(); GroupIter != GroupJson.end(); ++GroupIter)
	{
		wstring wstrGroupName = CStringUtility::ToWideString((*GroupIter)["GroupName"].asString());

		Json::Value MemberValue = (*GroupIter)["MemberAnimation"];

		for (auto MemberIter = MemberValue.begin(); MemberIter != MemberValue.end(); ++MemberIter)
		{
			wstring wstrAnimName = CStringUtility::ToWideString((*MemberIter).asString());
			int32 iAnimID = findAnimationID(wstrAnimName);

			if (iAnimID == -1)
				continue;

			// 애니메이션 그룹에 해당하는 애니메이션에 그룹 등록
			m_umapAnimations[iAnimID]->SetAnimGroup(wstrGroupName, iCnt);
		}

		++iCnt;
	}

	ifAnimGroup.close();
}

int32 Engine::CModel::findAnimationID(const wstring& _wstrAnimationName)
{
	wstring wstrFilePath = L"../../Resource/Models/AnimationIndexJson/" + m_wstrModelName + L".json";

	Json::Value AnimList;
	ifstream istream(wstrFilePath);
	if (istream.fail())
	{
		return -1;
	}

	istream >> AnimList;

	int iCnt = 0;
	for (auto animationName = AnimList.begin(); animationName != AnimList.end(); ++animationName)
	{
		if (_wstrAnimationName == CStringUtility::ToWideString((*animationName).asString()))
		{
			m_umapAnimatoinNameID.emplace(_wstrAnimationName, iCnt);
			return iCnt;
		}

		++iCnt;
	}

	return -1;
}

void Engine::CModel::RequestInstanceRender(const Matrix& _matWorld, ERenderType _eRenderType)
{
	for (auto& Pair : m_umapMeshInfos)
	{
		for (auto& tMeshInfo : Pair.second)
		{
			tMeshInfo->m_spMeshBuffer->AddRenderInstance(tMeshInfo->m_matTransform, _matWorld, _eRenderType);
		}
	}
}

void Engine::CModel::PlayAnimation(float _fDeltaSeconds)
{
	if (!m_bUseAnimation || m_bStopAnimation) { return; }

	// RefBone을 사용한다면 애니메이션을 업데이트하지 않습니다.
	if (m_bUseRefBones)
	{
		for (auto& iNoRefBoneIndex : m_lstNoRefBoneIndices)
		{
			m_vecBones[iNoRefBoneIndex]->InvalidateCombinedTransformationMatrix(_fDeltaSeconds, m_vecBones);
		}
		return;
	}

	PlayDownAnim(_fDeltaSeconds);

	PlayUpperAnim(_fDeltaSeconds);

	ResetChannel();

	for (auto& BoneRotation : m_lstBoneRotation)
	{
		m_vecBones[BoneRotation.first]->BlendRotation(BoneRotation.second);
	}

	m_lstBoneRotation.clear();

	PlayShake(_fDeltaSeconds);

	InvalidateCombinedMatrix(m_vecBones[0], _fDeltaSeconds);

	m_iExceptRotationBoneIndex = -1;
}

void Engine::CModel::ChangeAnimation(int32 _iNextAnimID, bool _bPlayReverse, float _fLoopStartRate)
{
	ChangeUpperAnimation(_iNextAnimID, _bPlayReverse, _fLoopStartRate);
	ChangeDownAnimation(_iNextAnimID, _bPlayReverse, _fLoopStartRate);
}

void Engine::CModel::ChangeAnimation(int32 _iNextAnimID, float _fLerpTime, bool _bPlayReverse, float _fLoopStartRate)
{
	ChangeUpperAnimation(_iNextAnimID, _fLerpTime, _bPlayReverse, _fLoopStartRate);
	ChangeDownAnimation(_iNextAnimID, _fLerpTime, _bPlayReverse, _fLoopStartRate);
}

void Engine::CModel::ChangeAnimation(const wstring& _wstrAnimName)
{
	if (!m_umapAnimatoinNameID.contains(_wstrAnimName)) { return; }

	int32 iID = m_umapAnimatoinNameID[_wstrAnimName];

	ChangeAnimation(iID, 0.f, false, 0.f);
}

void Engine::CModel::ChangeDifferentAnimation(int32 _iNextAnimID, bool _bPlayReverse, float _fLoopStartRate)
{
	if ((m_tCurrentDownAnimDat.m_iAnimID == _iNextAnimID && m_tCurrentDownAnimDat.m_bUseReversePlay == _bPlayReverse) ||
		(m_tCurrentUpperAnimDat.m_iAnimID == _iNextAnimID && m_tCurrentUpperAnimDat.m_bUseReversePlay == _bPlayReverse))
	{
		return;
	}

	ChangeAnimation(_iNextAnimID, _bPlayReverse, _fLoopStartRate);
}

void Engine::CModel::ChangeUpperAnimation(int32 _iNextAnimID, bool _bPlayReverse, float _fLoopStartRate)
{
	if (m_tCurrentUpperAnimDat.m_iAnimID == -1)
	{
		m_tCurrentUpperAnimDat.m_iAnimID = _iNextAnimID;
		m_tCurrentUpperAnimDat.m_bUseReversePlay = _bPlayReverse;
		return;
	}

	m_fUpperAnimStartLoopRate = _fLoopStartRate;

	if (m_bUpperAnimChangeFlag)
	{
		m_umapAnimations[m_tCurrentUpperAnimDat.m_iAnimID]->ResetLerpChecker(m_lstUpperBoneIndices);
	}

	if (m_tCurrentDownAnimDat.m_iAnimID != _iNextAnimID &&
		m_umapAnimations[m_tCurrentDownAnimDat.m_iAnimID]->IsSameGroup(m_umapAnimations[_iNextAnimID]))
	{
		m_umapAnimations[_iNextAnimID]->SetTrackPositionWithRatio(GetCurrentDownFrameRatio());
	}

	m_bUpperAnimChangeFlag = true;
	m_bUpperLoopEnd = false;
	m_iNumUpperCurrentAnimLoop = 0;
	m_tNextUpperAnimDat = FAnimDat(_iNextAnimID, _bPlayReverse);
}

void Engine::CModel::ChangeUpperAnimation(int32 _iNextAnimID, float _fLerpTime, bool _bPlayReverse, float _fLoopStartRate)
{
	if (m_tCurrentUpperAnimDat.m_iAnimID == -1)
	{
		m_tCurrentUpperAnimDat.m_iAnimID = _iNextAnimID;
		m_tCurrentUpperAnimDat.m_bUseReversePlay = _bPlayReverse;
		return;
	}

	ChangeUpperAnimation(_iNextAnimID, _bPlayReverse, _fLoopStartRate);

	m_umapAnimations[m_tCurrentUpperAnimDat.m_iAnimID]->SetLerpTime(_fLerpTime, m_lstUpperBoneIndices);
}

void Engine::CModel::ChangeDownAnimation(int32 _iNextAnimID, bool _bPlayReverse, float _fLoopStartRate)
{
	if (m_tCurrentDownAnimDat.m_iAnimID == -1)
	{
		m_tCurrentDownAnimDat.m_iAnimID = _iNextAnimID;
		m_tCurrentDownAnimDat.m_bUseReversePlay = _bPlayReverse;
		return;
	}

	m_fDownAnimStartLoopRate = _fLoopStartRate;

	if (m_bDownAnimChangeFlag)
	{
		m_umapAnimations[m_tCurrentDownAnimDat.m_iAnimID]->ResetLerpChecker(m_lstDownBoneIndices);
	}

	if (m_tCurrentUpperAnimDat.m_iAnimID != _iNextAnimID &&
		m_umapAnimations[m_tCurrentUpperAnimDat.m_iAnimID]->IsSameGroup(m_umapAnimations[_iNextAnimID]))
	{
		m_umapAnimations[_iNextAnimID]->SetTrackPositionWithRatio(GetCurrentDownFrameRatio());
	}

	m_bDownAnimChangeFlag = true;
	m_bDownLoopEnd = false;
	m_iNumDownCurrentAnimLoop = 0;
	m_tNextDownAnimDat = FAnimDat(_iNextAnimID, _bPlayReverse);
}

void Engine::CModel::ChangeDownAnimation(int32 _iNextAnimID, float _fLerpTime, bool _bPlayReverse, float _fLoopStartRate)
{
	if (m_tCurrentDownAnimDat.m_iAnimID == -1)
	{
		m_tCurrentDownAnimDat.m_iAnimID = _iNextAnimID;
		m_tCurrentDownAnimDat.m_bUseReversePlay = _bPlayReverse;
		return;
	}

	ChangeDownAnimation(_iNextAnimID, _bPlayReverse, _fLoopStartRate);

	m_umapAnimations[m_tCurrentDownAnimDat.m_iAnimID]->SetLerpTime(_fLerpTime, m_lstDownBoneIndices);
}

void Engine::CModel::SetBoneRotation(const wstring& _wstrBoneName, Vector3 _vRotation)
{
	m_lstBoneRotation.push_back({ GetBoneIndex(_wstrBoneName), _vRotation });
}

void Engine::CModel::ShakeUpperBody(float _fShakePower, float _fShakeTime)
{
	m_bShakeUpperBody = true;
	m_fShakePower = _fShakePower;
	m_fShakeTime = _fShakeTime;
}

void Engine::CModel::PlayUpperAnim(float _fDeltaSeconds)
{
	m_bUpperLoopEnd = false;

	// 애니메이션 보간
	if (m_bUpperAnimChangeFlag)
	{
		int32 iCurrentAnim = m_tCurrentUpperAnimDat.m_iAnimID;
		int32 iNextAnim = m_tNextUpperAnimDat.m_iAnimID;

		bool bResult = true;

		// 현재 애니메이션 재생
		if (iCurrentAnim == m_tCurrentDownAnimDat.m_iAnimID || iCurrentAnim == m_tNextDownAnimDat.m_iAnimID)
		{
			m_umapAnimations[iCurrentAnim]->PlayAnimationInLerp(0.f, m_tCurrentUpperAnimDat.m_bUseReversePlay, m_lstUpperBoneIndices);
		}
		else
		{
			m_umapAnimations[iCurrentAnim]->PlayAnimationInLerp(_fDeltaSeconds, m_tCurrentUpperAnimDat.m_bUseReversePlay, m_lstUpperBoneIndices);
		}

		// 다음 애니메이션 재생
		if (iNextAnim == m_tCurrentDownAnimDat.m_iAnimID || iNextAnim == m_tNextDownAnimDat.m_iAnimID || iNextAnim == iCurrentAnim)
		{
			m_umapAnimations[iNextAnim]->PlayAnimation(0.f, m_tCurrentUpperAnimDat.m_bUseReversePlay, m_lstUpperBoneIndices, m_fUpperAnimStartLoopRate);
		}
		else
		{
			m_umapAnimations[iNextAnim]->PlayAnimation(_fDeltaSeconds, m_tCurrentUpperAnimDat.m_bUseReversePlay, m_lstUpperBoneIndices, m_fUpperAnimStartLoopRate);
		}

		// 선형보간
		bResult = m_umapAnimations[iCurrentAnim]->ChangeAnimationLerp(_fDeltaSeconds, m_umapAnimations[iNextAnim], m_tCurrentUpperAnimDat.m_bUseReversePlay, m_lstUpperBoneIndices);
		if (bResult)
		{
			m_bUpperLerpEnd = true;
		}
	}
	else
	{
		int32 iCurrentAnim = m_tCurrentUpperAnimDat.m_iAnimID;
		if (iCurrentAnim == m_tCurrentDownAnimDat.m_iAnimID || (m_bDownAnimChangeFlag && iCurrentAnim == m_tNextDownAnimDat.m_iAnimID)) { _fDeltaSeconds = 0.0f; }

		if (iCurrentAnim == m_tCurrentDownAnimDat.m_iAnimID) { m_bUpperLoopEnd = m_bDownLoopEnd; }

		// 애니메이션 재생
		if (m_umapAnimations[iCurrentAnim]->PlayAnimation(_fDeltaSeconds, m_tCurrentUpperAnimDat.m_bUseReversePlay, m_lstUpperBoneIndices, m_fUpperAnimStartLoopRate))
		{
			++m_iNumUpperCurrentAnimLoop;
			m_bUpperLoopEnd = true;
		}
	}
}

void Engine::CModel::PlayDownAnim(float _fDeltaSeconds)
{
	m_bDownLoopEnd = false;

	// 애니메이션 보간
	if (m_bDownAnimChangeFlag)
	{
		int32 iCurrentAnim = m_tCurrentDownAnimDat.m_iAnimID;
		int32 iNextAnim = m_tNextDownAnimDat.m_iAnimID;

		bool bResult = true;
		m_umapAnimations[iCurrentAnim]->PlayAnimationInLerp(_fDeltaSeconds, m_tCurrentDownAnimDat.m_bUseReversePlay, m_lstDownBoneIndices);

		if (iCurrentAnim != iNextAnim)
		{
			m_umapAnimations[iNextAnim]->PlayAnimation(_fDeltaSeconds, m_tCurrentDownAnimDat.m_bUseReversePlay, m_lstDownBoneIndices, m_fDownAnimStartLoopRate);
		}

		bResult = m_umapAnimations[iCurrentAnim]->ChangeAnimationLerp(_fDeltaSeconds, m_umapAnimations[iNextAnim], m_tCurrentDownAnimDat.m_bUseReversePlay, m_lstDownBoneIndices);

		if (bResult)
		{
			m_bDownLerpEnd = true;
		}
	}
	else
	{
		int32 iCurrentAnim = m_tCurrentDownAnimDat.m_iAnimID;

		// 애니메이션 재생
		if (m_umapAnimations[iCurrentAnim]->PlayAnimation(_fDeltaSeconds, m_tCurrentDownAnimDat.m_bUseReversePlay, m_lstDownBoneIndices, m_fDownAnimStartLoopRate))
		{
			++m_iNumDownCurrentAnimLoop;
			m_bDownLoopEnd = true;
		}
	}
}

void Engine::CModel::SetBoneGroup(shared_ptr<CBone> _spRootBone, list<int32>& _lstBoneGroup)
{
	int32 iBoneIndex = _spRootBone->GetMyIndex();
	_lstBoneGroup.push_back(iBoneIndex);

	auto lstChildBone = _spRootBone->GetChildBoneIndices();

	if (lstChildBone.empty()) { return; }

	for (int32 iChildBoneIndex : lstChildBone)
	{
		SetBoneGroup(m_vecBones[iChildBoneIndex], _lstBoneGroup);
	}
}

void Engine::CModel::ExChangeBonesWithParent(const shared_ptr<CModel> _spParentModel)
{
	for (auto& spBone : m_vecBones)
	{
		wstring wstrBoneName = spBone->GetBoneName();
		int32 iBoneIndex = _spParentModel->GetBoneIndex(wstrBoneName);

		// 부모 본으로 교체
		if (iBoneIndex != -1 && wstrBoneName != L"eyes_r" && wstrBoneName != L"eyes_l")
		{
			spBone = _spParentModel->GetBones()[iBoneIndex];
		}

		// 참조할 본이 없는 경우 따로 등록해주고 계산이 이루어지도록 한다
		else
		{
			m_lstNoRefBoneIndices.push_back(spBone->GetMyIndex());
		}
	}

}

void Engine::CModel::ResetChannel()
{
	if (m_bDownLerpEnd)
	{
		int32 iCurrentAnim = m_tCurrentDownAnimDat.m_iAnimID;
		int32 iNextAnim = m_tNextDownAnimDat.m_iAnimID;

		// 다음애니메이션 같지 않으면서 상체가 같은 애니메이션을 사용하지 않는다면 채널 리셋
		if (m_tNextUpperAnimDat.m_iAnimID != iCurrentAnim &&
			iCurrentAnim != iNextAnim)
		{
			m_umapAnimations[iCurrentAnim]->ResetChannel();
		}

		m_tCurrentDownAnimDat = m_tNextDownAnimDat;
		m_bDownAnimChangeFlag = false;
		m_bDownLerpEnd = false;
	}

	if (m_bUpperLerpEnd)
	{
		int32 iCurrentAnim = m_tCurrentUpperAnimDat.m_iAnimID;
		int32 iNextAnim = m_tNextUpperAnimDat.m_iAnimID;

		if (iCurrentAnim != iNextAnim &&
			m_tCurrentDownAnimDat.m_iAnimID != iCurrentAnim)
		{
			m_umapAnimations[iCurrentAnim]->ResetChannel();
		}

		m_tCurrentUpperAnimDat = m_tNextUpperAnimDat;
		m_bUpperAnimChangeFlag = false;
		m_bUpperLerpEnd = false;
	}
}

void Engine::CModel::InvalidateCombinedMatrix(shared_ptr<CBone> _spRootBone, float _fDeltaSeconds)
{
	if (_spRootBone->GetMyIndex() == m_iExceptRotationBoneIndex)
	{
		_spRootBone->InvalidateCombinedTransformationMatrixExceptRotation(m_vecBones);
	}
	else
	{
		_spRootBone->InvalidateCombinedTransformationMatrix(_fDeltaSeconds, m_vecBones);
	}

	for (auto& iChildIndex : _spRootBone->GetChildBoneIndices())
	{
		InvalidateCombinedMatrix(m_vecBones[iChildIndex], _fDeltaSeconds);
	}
}

void Engine::CModel::PlayShake(float _fDeltaSeconds)
{
	if (m_iExceptRotationBoneIndex)

		if (m_bShakeUpperBody)
		{
			m_fShakeCheck += _fDeltaSeconds;

			if (m_fShakeCheck >= m_fShakeTime)
			{
				m_bShakeUpperBody = false;
				m_fShakeCheck = 0.f;
				return;
			}

			m_vecBones[m_iSpineBoneIndex]->ShakeBone(m_fShakePower);
		}
}
