#include "pch.h"
#include "Bone.h"
#include "MathUtility.h"

shared_ptr<CBone> Engine::CBone::Create(ifstream& _ifFile, Matrix _matPivot)
{
	shared_ptr<CBone> spInstance = make_shared<CBone>();

	CHECKF(spInstance != nullptr, L"CBone Create Failed");
	spInstance->Initialize(_ifFile, _matPivot);

	return spInstance;
}

void Engine::CBone::Initialize(ifstream& _ifFile, Matrix _matPivot)
{
	int32 iBoneNameLen = 0;
	wchar_t wszBoneName[MAX_PATH] = {};
	_ifFile.read(reinterpret_cast<char*>(&iBoneNameLen), sizeof(iBoneNameLen));
	_ifFile.read(reinterpret_cast<char*>(wszBoneName), iBoneNameLen * sizeof(wchar_t));
	m_wstrBoneName = wszBoneName;

	_ifFile.read(reinterpret_cast<char*>(&m_iParentBoneIndex), sizeof(m_iParentBoneIndex));
	_ifFile.read(reinterpret_cast<char*>(&m_iMyIndex), sizeof(m_iMyIndex));

	int32 iChildBoneSize = 0;
	_ifFile.read(reinterpret_cast<char*>(&iChildBoneSize), sizeof(iChildBoneSize));
	for (size_t i = 0; i < iChildBoneSize; ++i)
	{
		int32 iChildIndex = 0;
		_ifFile.read(reinterpret_cast<char*>(&iChildIndex), sizeof(iChildIndex));
		m_lstChildBoneIndices.push_back(iChildIndex);
	}

	for (size_t i = 0; i < 4; ++i)
	{
		for (size_t j = 0; j < 4; ++j)
		{
			_ifFile.read(reinterpret_cast<char*>(&m_matTransformation.m[i][j]), sizeof(m_matTransformation.m[i][j]));
		}
	}

	if (m_iMyIndex == 0)
	{
		m_bIsRootBone = true;
		m_matTransformation = m_matTransformation * _matPivot;
	}
	m_matPivot = _matPivot;
	m_matInitialTransformation = m_matTransformation;
}

void Engine::CBone::Release()
{
}

void Engine::CBone::BlendRotation(Vector3 _vRotation)
{
	Matrix matRotX = Matrix::CreateFromQuaternion(Quaternion::CreateFromAxisAngle(Vector3(1.f, 0.f, 0.f), _vRotation.y));
	Matrix matRotY = Matrix::CreateFromQuaternion(Quaternion::CreateFromAxisAngle(Vector3(0.f, 1.f, 0.f), -_vRotation.z));
	Matrix matRotZ = Matrix::CreateFromQuaternion(Quaternion::CreateFromAxisAngle(Vector3(0.f, 0.f, 1.f), -_vRotation.x));

	Matrix matRotaiton = matRotX * matRotY * matRotZ;

	m_matTransformation *= matRotaiton;
}

void Engine::CBone::ShakeBone(float _fPower)
{
	float fShakeX = CMathUtility::GetRandomFloat(0.1f, _fPower);
	//float fShakeY = CMathUtility::GetRandomFloat(0.1f, _fPower);
	float fShakeZ = CMathUtility::GetRandomFloat(0.1f, _fPower);

	Vector3 vCurrentPos = m_matTransformation.Translation();
	vCurrentPos += Vector3(fShakeX, 0.f, fShakeZ);

	m_matTransformation.Translation(vCurrentPos);
}

void Engine::CBone::SetMatrixToInitial()
{
	m_matTransformation = m_matInitialTransformation;
}

void Engine::CBone::InvalidateCombinedTransformationMatrix(float _fDeltaSeconds, const vector<shared_ptr<CBone>>& _vecBones)
{
	if (m_bIsRootBone)
		m_matCombinedTransformation = m_matTransformation;
	else
	{
		if (m_bRotationExcepted)
		{
			m_fCheckTime += _fDeltaSeconds;
			float fRatio = m_fCheckTime / m_fLerpTime;
			if (fRatio > 1.f) { fRatio = 1.f; }

			Matrix matParentCombined = _vecBones[m_iParentBoneIndex]->m_matCombinedTransformation;
			Vector3 vTranslation = matParentCombined.Translation();

			matParentCombined.Translation(Vector3(0.f, 0.f, 0.f));

			m_matPrevParentCombinedTransformation.Translation(Vector3(0.f, 0.f, 0.f));

			m_matPrevParentCombinedTransformation = Matrix::Lerp(m_matPrevParentCombinedTransformation, matParentCombined, fRatio);
			m_matPrevParentCombinedTransformation.Translation(vTranslation);
			m_matCombinedTransformation = m_matTransformation * m_matPrevParentCombinedTransformation;

			if (fRatio == 1.f) { m_bRotationExcepted = false; }
		}
		else
		{
			m_matCombinedTransformation = m_matTransformation * _vecBones[m_iParentBoneIndex]->m_matCombinedTransformation;
			m_matPrevParentCombinedTransformation = _vecBones[m_iParentBoneIndex]->m_matCombinedTransformation;
		}
	}
}

void Engine::CBone::InvalidateCombinedTransformationMatrixExceptRotation(const vector<shared_ptr<CBone>>& _vecBones)
{
	m_bRotationExcepted = true;
	m_fCheckTime = 0.f;

	Matrix matParentCombined = _vecBones[m_iParentBoneIndex]->m_matCombinedTransformation;

	float fScaleX = matParentCombined.Right().Length();
	float fScaleY = matParentCombined.Up().Length();
	float fScaleZ = matParentCombined.Backward().Length();

	Matrix matSale = Matrix::CreateScale(fScaleX, fScaleY, fScaleZ);
	Matrix matRot = Matrix::CreateRotationZ(XMConvertToRadians(90.f)) * Matrix::CreateRotationY(XMConvertToRadians(-65.f));

	Matrix matCoumputedCombined;

	matCoumputedCombined = matSale * matRot;

	m_matPrevParentCombinedTransformation.Translation(Vector3(0.f, 0.f, 0.f));
	m_matPrevParentCombinedTransformation = Matrix::Lerp(m_matPrevParentCombinedTransformation, matCoumputedCombined, 0.2f);
	m_matPrevParentCombinedTransformation.Translation(matParentCombined.Translation());

	m_matCombinedTransformation = m_matTransformation * m_matPrevParentCombinedTransformation;
}