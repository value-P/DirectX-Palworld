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
		// 특정 본에 회전을 적용할때 루트본의 좌표계가 적용되지 않아 축 틀어짐이 발생할 수 있으므로
		// 저장을 해놓고 Combined행렬이 한번번 곱해진 상태에 적용해야한다
		bool			m_bUseRotation = false;

		bool			m_bIsRootBone = false;
		wstring			m_wstrBoneName = L"";

		int32			m_iParentBoneIndex = 0;
		int32			m_iMyIndex = 0;
		list<int32>		m_lstChildBoneIndices;

		Matrix			m_matPivot;
		Matrix			m_matInitialTransformation;		// 가장 초기 상태의 상태행렬
		Matrix			m_matTransformation;			// 부모기준으로 표현된 자신만의 상태행렬
		Matrix			m_matCombinedTransformation;	// 내 transformationMatrix * 부모 combinedTransformationMatrix

		// 회전값 고정 풀릴 시 보간에 사용할 변수
		bool			m_bRotationExcepted = false;
		float			m_fLerpTime = 2.f;
		float			m_fCheckTime = 0.f;

		// 회전값 고정 시 보간에 사용할 Matrix
		Matrix			m_matPrevParentCombinedTransformation;
	};
}
