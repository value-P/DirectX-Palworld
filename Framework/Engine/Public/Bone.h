#pragma once

namespace Engine
{
	class CBone final
	{
		/********************
			Ctor/Dtor
		********************/
	public:
		CBone() = default;
		virtual ~CBone() = default;

		/********************
			Getter
		********************/
	public:
		bool IsRootBone() { return m_bIsRootBone; }
		wstring GetBoneName() { return m_wstrBoneName; }
		Matrix GetCombinedTransformationMatrix() { return m_matCombinedTransformation; }
		Matrix GetInitialTransformationMatrix() { return m_matInitialTransformation; }
		int32 GetParentBoneIndex() { return m_iParentBoneIndex; }
		int32 GetMyIndex() { return m_iMyIndex; }
		const list<int32>& GetChildBoneIndices() { return m_lstChildBoneIndices; }

		/********************
			Setter
		********************/
		void SetTransformationMatrix(const Matrix& _matTransformation)
		{
			if (m_bIsRootBone)
				m_matTransformation = _matTransformation * m_matPivot;
			else
				m_matTransformation = _matTransformation;
		}

		void BlendRotation(Vector3 _vRotation);

		void ShakeBone(float _fPower);

		void SetParentBoneIndex(int32 _iParentBoneIndex) { m_iParentBoneIndex = _iParentBoneIndex; }

		void SetMatrixToInitial();

		/********************
			Events
		********************/
	public:
		static shared_ptr<CBone> Create(ifstream& _ifFile, Matrix _matPivot);
		virtual void Initialize(ifstream& _ifFile, Matrix _matPivot);
		void Release();

		/********************
			Methods
		********************/
	public:
		void InvalidateCombinedTransformationMatrix(float _fDeltaSeconds, const vector<shared_ptr<CBone>>& _vecBones);
		void InvalidateCombinedTransformationMatrixExceptRotation(const vector<shared_ptr<CBone>>& _vecBones);

		/********************
			Data Members
		********************/
	private:
		// Ư�� ���� ȸ���� �����Ҷ� ��Ʈ���� ��ǥ�谡 ������� �ʾ� �� Ʋ������ �߻��� �� �����Ƿ�
		// ������ �س��� Combined����� �ѹ��� ������ ���¿� �����ؾ��Ѵ�
		bool			m_bUseRotation = false;

		bool			m_bIsRootBone = false;
		wstring			m_wstrBoneName = L"";

		int32			m_iParentBoneIndex = 0;
		int32			m_iMyIndex = 0;
		list<int32>		m_lstChildBoneIndices;

		Matrix			m_matPivot;
		Matrix			m_matInitialTransformation;		// ���� �ʱ� ������ �������
		Matrix			m_matTransformation;			// �θ�������� ǥ���� �ڽŸ��� �������
		Matrix			m_matCombinedTransformation;	// �� transformationMatrix * �θ� combinedTransformationMatrix

		// ȸ���� ���� Ǯ�� �� ������ ����� ����
		bool			m_bRotationExcepted = false;
		float			m_fLerpTime = 2.f;
		float			m_fCheckTime = 0.f;

		// ȸ���� ���� �� ������ ����� Matrix
		Matrix			m_matPrevParentCombinedTransformation;
	};
}
