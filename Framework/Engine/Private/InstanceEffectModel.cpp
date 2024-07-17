#include "pch.h"
#include "InstanceEffectModel.h"

#include "Bone.h"
#include "Animation.h"
#include "Material.h"
#include "InstanceEffectMeshBuffer.h"

shared_ptr<CInstanceEffectModel> Engine::CInstanceEffectModel::Create(const wstring& _wstrDatPathFile, Matrix _matPivot, const shared_ptr<CModel> _spParentModel, FInstanceEffectMeshInfo _tInstanceEffectMeshInfo)
{
	shared_ptr<CInstanceEffectModel> spInstanceModel = make_shared<CInstanceEffectModel>();
	ENSUREF(spInstanceModel->Initialize(_wstrDatPathFile, _matPivot, _spParentModel, _tInstanceEffectMeshInfo), L"Failed to initialize instance model.");
	return spInstanceModel;
}

HRESULT Engine::CInstanceEffectModel::Initialize(const wstring& _wstrDatPathFile, Matrix _matPivot, const shared_ptr<CModel> _spParentModel, FInstanceEffectMeshInfo _tInstanceEffectMeshInfo)
{
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

		Matrix matPivot = _matPivot;
		if (m_eModelType == EModelType::ANIM)
		{
			Vector3 vScale, vPosition;
			Quaternion qRotation;
			_matPivot.Decompose(vScale, qRotation, vPosition);
			matPivot = Matrix::CreateScale(vScale) * Matrix::CreateTranslation(vPosition);
		}
		matTransform *= matPivot;


		for (int32 i = 0; i < iNumObjectMeshes; ++i)
		{
			ifstream ifMeshDatFile(L"../../Resource/Models/Meshes/" + vecMeshNames[i] + L".dat", std::ios::binary);
			shared_ptr<CInstanceEffectMeshBuffer> spMeshBuffer;

			spMeshBuffer = CInstanceEffectMeshBuffer::Create(_wstrDatPathFile, ifMeshDatFile, _matPivot, m_eModelType, _tInstanceEffectMeshInfo);
			spMeshBuffer->SetMeshName(vecMeshNames[i]);

			FMeshInfo tMeshInfo;
			tMeshInfo.m_matTransform = matTransform;
			tMeshInfo.m_spMeshBuffer = spMeshBuffer;

			EShaderType eShaderType = spMeshBuffer->GetMaterial()->GetShaderType();
			m_umapMeshInfos[eShaderType].push_back(make_shared<FMeshInfo>(tMeshInfo));

			ifMeshDatFile.close();
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

	for (auto& Pair : m_umapMeshInfos)
	{
		for (auto& tMeshInfo : Pair.second)
		{
			m_vecMeshBuffers.push_back(tMeshInfo->m_spMeshBuffer);
		}
	}

	return S_OK;
}

ComPtr<ID3D11Buffer>& Engine::CInstanceEffectModel::GetInstanceBuffer()
{
	CHECKF(!m_vecMeshBuffers.empty(), L"m_vecMeshBuffers is empty! : CInstanceEffectModel");
	return static_pointer_cast<CInstanceEffectMeshBuffer>(m_vecMeshBuffers[0])->GetInstanceBuffer();
}