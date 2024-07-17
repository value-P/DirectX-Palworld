#pragma once

namespace Engine
{
	class CMeshBuffer;
	class CBone;
	class CMaterial;
	class CAnimation;
	class CSphereComponent;
	class CComponent;

	class ENGINE_DLL CModel : public enable_shared_from_this<CModel>
	{
		/********************
			Struct
		********************/
	protected:
		struct FAnimDat
		{
			int32 m_iAnimID = -1; // �ִϸ��̼� ID : client�� Enum��

			bool m_bUseReversePlay = false; // ����� �ɼ�
		};
		struct FMeshInfo
		{
			Matrix m_matTransform;
			shared_ptr<CMeshBuffer> m_spMeshBuffer;
		};
		struct FColliderInfo
		{
			EColliderType m_eColliderType = EColliderType::ENUM_END;	// Ÿ��
			wstring       m_wstrColliderName = L"";						// �̸�
			wstring       m_wstrParentBoneName = L"";					// �θ� �� �̸�
			Vector3       m_vCenter = Vector3::Zero;					// ���� - ��� ��ǥ
			Vector3       m_vRotationYPR = Vector3::Zero;				// ȸ�� - YawPitchRoll ������ XYZ�� ����.
			float         m_fRadius = 0;								// ������ - Sphere, Sphy
			float         m_fLength = 0;								// ���� - Sphy
		};

		/********************
			Ctor/Dtor
		********************/
	public:
		CModel() = default;

		CModel(const CModel& rhs) = delete;

		virtual ~CModel() = default;

		/********************
			Getter/Setter
		********************/
	public: // �ִϸ��̼��� ������ �⺻ ������ ����
		const wstring& GetModelName() { return m_wstrModelName; }

		EModelType GetModelType() { return m_eModelType; }

		EMeshManageType GetMeshType() { return m_eMeshType; }

		int32 GetNumColliders() { return (int32)m_vecColliderInfos.size(); }
		shared_ptr<FColliderInfo> GetColliderInfo(int32 _iColliderIndex) { return m_vecColliderInfos[_iColliderIndex]; }
		shared_ptr<FColliderInfo> GetColliderInfo(wstring _wstrColliderName);
		const vector<shared_ptr<FColliderInfo>>& GetColliderInfos() { return m_vecColliderInfos; }

		int32 GetNumShaders() { return (int32)m_vecShaderTypes.size(); }

		int32 GetNumMeshes(EShaderType _eShaderType) { return (int32)m_umapMeshInfos[_eShaderType].size(); };
		const unordered_map<EShaderType, vector<shared_ptr<FMeshInfo>>>& GetMeshInfos() { return m_umapMeshInfos; }

	public:
		int32 GetBoneIndex(const wstring& _wstrBoneName);

		const vector<shared_ptr<CBone>>& GetBones() { return m_vecBones; }

		float GetCurrentUpperFrameRatio();
		float GetCurrentDownFrameRatio();

		int32 GetCurrentUpperAnimID() { return m_tCurrentUpperAnimDat.m_iAnimID; }
		int32 GetCurrentDownAnimID() { return m_tCurrentDownAnimDat.m_iAnimID; }

		list<FAnimNotifyData> GetCurrentUpperAnimNotifyData();
		list<FAnimNotifyData> GetCurrentDownAnimNotifyData();

		shared_ptr<CBone> GetBonePtr(const wstring& _wstrBoneName);
		shared_ptr<CBone> GetBonePtr(int32 _iBoneIndex);

		int32 GetUpperCurrentLoop() { return m_iNumUpperCurrentAnimLoop; }
		int32 GetDownCurrentLoop() { return m_iNumDownCurrentAnimLoop; }

		FModelAnimData GetModelAnimationData();

		vector<wstring> GetAnimationNames();

		wstring GetAnimationName(int32 _iAnimID);

		void SetUseAnimation(bool _bUseAnimation, bool _bUseIdentityMatrices);
		void SetRotationExceptBone(const wstring& _wstrBoneName);
		void SetStopAnimation(bool _bStopAnimation) { m_bStopAnimation = _bStopAnimation; }
		void SetTrackPosition(float _fTrackPosition);

		bool IsDownLerping() { return m_bDownAnimChangeFlag; }
		bool IsUpperLerping() { return m_bUpperAnimChangeFlag; }

		/********************
			Events
		********************/
	public:
		static shared_ptr<CModel> Create(const wstring& _wstrDatPathFile, Matrix _matPivot, EMeshManageType _eMeshManageType, const shared_ptr<CModel> _spParentModel, const wstring& _wstrUpperBoneName = L"", const wstring& _wstrAnimGroupJsonPath = L"");

		HRESULT Initialize(const wstring& _wstrDatPathFile, Matrix _matPivot, EMeshManageType _eMeshManageType, const shared_ptr<CModel> _spParentModel, const wstring& _wstrUpperBoneName, const wstring& _wstrAnimGroupJsonPath);

		void Release();

	protected:
		void InitBoneGroup(const wstring& _wstrUpperBoneName);

		void InitAnimGroup(const wstring& _wstrAnimGroupJsonPath);

		int32 findAnimationID(const wstring& _wstrAnimationName);

		/********************
			Methods
		********************/
	public: // Render ����
		void RequestInstanceRender(const Matrix& _matWorld, ERenderType _eRenderType);

	public:
		void PlayAnimation(float _fDeltaSeconds);

		// �ִϸ��̼� ����
		void ChangeAnimation(int32 _iNextAnim, bool _bPlayReverse, float _fLoopStartRate);
		void ChangeAnimation(int32 _iNextAnim, float _fLerpTime, bool _bPlayReverse, float _fLoopStartRate);
		void ChangeAnimation(const wstring& _wstrAnimName);
		void ChangeDifferentAnimation(int32 _iNextAnim, bool _bPlayReverse, float _fLoopStartRate);

		// ��ü ����
		void ChangeUpperAnimation(int32 _iNextAnim, bool _bPlayReverse, float _fLoopStartRate);
		void ChangeUpperAnimation(int32 _iNextAnim, float _fLerpTime, bool _bPlayReverse, float _fLoopStartRate);

		// ��ü ����
		void ChangeDownAnimation(int32 _iNextAnim, bool _bPlayReverse, float _fLoopStartRate);
		void ChangeDownAnimation(int32 _iNextAnim, float _fLerpTime, bool _bPlayReverse, float _fLoopStartRate);

		// Ư�� �� ȸ�� ���̱�
		void SetBoneRotation(const wstring& _wstrBoneName, Vector3 _vRotation);

		// ��ü ����
		void ShakeUpperBody(float _fShakePower, float _fShakeTime);

	protected:
		void PlayUpperAnim(float _fDeltaSeconds);

		void PlayDownAnim(float _fDeltaSeconds);

		void SetBoneGroup(shared_ptr<CBone> _spRootBone, list<int32>& _lstBoneGroup);

		void ExChangeBonesWithParent(const shared_ptr<CModel> _spParentModel);

		void ResetChannel();

		void InvalidateCombinedMatrix(shared_ptr<CBone> _spRootBone, float _fDeltaSecond);

		void PlayShake(float _fDeltaSeconds);

		/********************
			Data Members
		********************/
	protected: // �⺻ �� ������
		wstring																m_wstrModelName = L"";
		EModelType															m_eModelType = EModelType::ENUM_END;
		EMeshManageType														m_eMeshType = EMeshManageType::ENUM_END;
		vector<shared_ptr<CBone>>											m_vecBones;					// ��
		unordered_map<int32, shared_ptr<CAnimation>>						m_umapAnimations;			// �ִϸ��̼�
		vector<shared_ptr<FColliderInfo>>									m_vecColliderInfos;			// �� �𵨿� �⺻���� ������ �ݶ��̴� ���
		vector<EShaderType>													m_vecShaderTypes;			// �� ���� ����ϴ� ���̴� ���� ���
		unordered_map<EShaderType, vector<shared_ptr<FMeshInfo>>>			m_umapMeshInfos;			// �޽� ���
		Matrix																m_matPivot;

	protected:
		weak_ptr<CComponent>										m_wpOwnerComponent;

		bool														m_bUseRefBones = false;

		// �ִϸ��̼� ��� ����
		bool														m_bUseAnimation = true;
		// �ִϸ��̼� ���� �� ���̴��� �׵ ���� ��
		bool														m_bUseIdentityMatrices = false;

		// �ִϸ��̼� ���� �ɼ�
		bool														m_bUpperAnimChangeFlag = false;
		bool														m_bDownAnimChangeFlag = false;

		bool														m_bUpperLerpEnd = false;
		bool														m_bDownLerpEnd = false;

		bool														m_bDownLoopEnd = false;
		bool														m_bUpperLoopEnd = false;

		// �ִϸ��̼� ���� �� �������������� ����
		float														m_fUpperAnimStartLoopRate = 0.f;
		float														m_fDownAnimStartLoopRate = 0.f;

		// �ִϸ��̼� ����
		FAnimDat													m_tNextUpperAnimDat;
		FAnimDat													m_tNextDownAnimDat;

		FAnimDat													m_tCurrentUpperAnimDat;
		FAnimDat													m_tCurrentDownAnimDat;

		FAnimDat													m_tPrevUpperAnimDat;
		FAnimDat													m_tPrevDownAnimDat;

		// ����ü�� ������ ������ �ε���(����ü �и����� ���� ��� ��ü���ε����� ������ ��)
		list<int32>													m_lstUpperBoneIndices;	// ��ü �� �ε��� ����
		list<int32>													m_lstDownBoneIndices;	// ��ü �� �ε��� ����

		// ���۷��� �� ���� ���۷����� �������� �ʴ� ������ �ε���
		list<int32>													m_lstNoRefBoneIndices;

		// InvalidateMatrix�� �Ҷ� �θ� combined��Ŀ��� ȸ���� ���� ���ؾ��� ���� �ε���
		int32														m_iExceptRotationBoneIndex = -1;

		// ȸ���� ������ ���ε����� ȸ����
		// �� ���� ȸ���� �����ϴ� ���� �� �ִϸ��̼� �������� �Ͼ�� �� ���̸� �ִϸ��̼��� ����ȴٸ� �̴� �����ǵ��� �� ���̴�
		list<pair<int32, Vector3>>									m_lstBoneRotation;

		// ��� ī��Ʈ
		int32														m_iNumUpperCurrentAnimLoop = 0;
		int32														m_iNumDownCurrentAnimLoop = 0;

		// ���� �ɼ�
		bool														m_bStopAnimation = false;

		// �̸��� �ε��� ��Ī�� umap
		unordered_map<wstring, int32>								m_umapAnimatoinNameID;

		// ��ü ����ŷ ��
		int32														m_iSpineBoneIndex = -1;

		bool														m_bShakeUpperBody = false;
		float														m_fShakePower = 0.f;
		float														m_fShakeTime = 0.f;
		float														m_fShakeCheck = 0.f;
	};
}