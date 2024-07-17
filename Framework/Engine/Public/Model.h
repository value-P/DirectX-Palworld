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
			int32 m_iAnimID = -1; // 애니메이션 ID : client의 Enum값

			bool m_bUseReversePlay = false; // 역재생 옵션
		};
		struct FMeshInfo
		{
			Matrix m_matTransform;
			shared_ptr<CMeshBuffer> m_spMeshBuffer;
		};
		struct FColliderInfo
		{
			EColliderType m_eColliderType = EColliderType::ENUM_END;	// 타입
			wstring       m_wstrColliderName = L"";						// 이름
			wstring       m_wstrParentBoneName = L"";					// 부모 본 이름
			Vector3       m_vCenter = Vector3::Zero;					// 중점 - 상대 좌표
			Vector3       m_vRotationYPR = Vector3::Zero;				// 회전 - YawPitchRoll 순서로 XYZ에 들어간다.
			float         m_fRadius = 0;								// 반지름 - Sphere, Sphy
			float         m_fLength = 0;								// 길이 - Sphy
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
	public: // 애니메이션을 제외한 기본 데이터 관련
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
	public: // Render 관련
		void RequestInstanceRender(const Matrix& _matWorld, ERenderType _eRenderType);

	public:
		void PlayAnimation(float _fDeltaSeconds);

		// 애니메이션 변경
		void ChangeAnimation(int32 _iNextAnim, bool _bPlayReverse, float _fLoopStartRate);
		void ChangeAnimation(int32 _iNextAnim, float _fLerpTime, bool _bPlayReverse, float _fLoopStartRate);
		void ChangeAnimation(const wstring& _wstrAnimName);
		void ChangeDifferentAnimation(int32 _iNextAnim, bool _bPlayReverse, float _fLoopStartRate);

		// 상체 변경
		void ChangeUpperAnimation(int32 _iNextAnim, bool _bPlayReverse, float _fLoopStartRate);
		void ChangeUpperAnimation(int32 _iNextAnim, float _fLerpTime, bool _bPlayReverse, float _fLoopStartRate);

		// 하체 변경
		void ChangeDownAnimation(int32 _iNextAnim, bool _bPlayReverse, float _fLoopStartRate);
		void ChangeDownAnimation(int32 _iNextAnim, float _fLerpTime, bool _bPlayReverse, float _fLoopStartRate);

		// 특정 본 회전 먹이기
		void SetBoneRotation(const wstring& _wstrBoneName, Vector3 _vRotation);

		// 상체 흔들기
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
	protected: // 기본 모델 데이터
		wstring																m_wstrModelName = L"";
		EModelType															m_eModelType = EModelType::ENUM_END;
		EMeshManageType														m_eMeshType = EMeshManageType::ENUM_END;
		vector<shared_ptr<CBone>>											m_vecBones;					// 뼈
		unordered_map<int32, shared_ptr<CAnimation>>						m_umapAnimations;			// 애니메이션
		vector<shared_ptr<FColliderInfo>>									m_vecColliderInfos;			// 이 모델에 기본으로 부착된 콜라이더 목록
		vector<EShaderType>													m_vecShaderTypes;			// 이 모델이 사용하는 쉐이더 종류 목록
		unordered_map<EShaderType, vector<shared_ptr<FMeshInfo>>>			m_umapMeshInfos;			// 메쉬 목록
		Matrix																m_matPivot;

	protected:
		weak_ptr<CComponent>										m_wpOwnerComponent;

		bool														m_bUseRefBones = false;

		// 애니메이션 사용 여부
		bool														m_bUseAnimation = true;
		// 애니메이션 비사용 시 쉐이더에 항등만 던질 지
		bool														m_bUseIdentityMatrices = false;

		// 애니메이션 변경 옵션
		bool														m_bUpperAnimChangeFlag = false;
		bool														m_bDownAnimChangeFlag = false;

		bool														m_bUpperLerpEnd = false;
		bool														m_bDownLerpEnd = false;

		bool														m_bDownLoopEnd = false;
		bool														m_bUpperLoopEnd = false;

		// 애니메이션 루프 시 시작지점에대한 비율
		float														m_fUpperAnimStartLoopRate = 0.f;
		float														m_fDownAnimStartLoopRate = 0.f;

		// 애니메이션 정보
		FAnimDat													m_tNextUpperAnimDat;
		FAnimDat													m_tNextDownAnimDat;

		FAnimDat													m_tCurrentUpperAnimDat;
		FAnimDat													m_tCurrentDownAnimDat;

		FAnimDat													m_tPrevUpperAnimDat;
		FAnimDat													m_tPrevDownAnimDat;

		// 상하체에 적용할 뼈들의 인덱스(상하체 분리하지 않을 경우 상체본인덱스에 모든것이 들어감)
		list<int32>													m_lstUpperBoneIndices;	// 상체 본 인덱스 묶음
		list<int32>													m_lstDownBoneIndices;	// 하체 본 인덱스 묶음

		// 레퍼런스 본 사용시 레퍼런스에 존재하지 않는 본들의 인덱스
		list<int32>													m_lstNoRefBoneIndices;

		// InvalidateMatrix를 할때 부모 combined행렬에서 회전을 빼고 곱해야할 본의 인덱스
		int32														m_iExceptRotationBoneIndex = -1;

		// 회전을 적용할 본인덱스와 회전값
		// ※ 본에 회전을 적용하는 것은 한 애니메이션 내에서만 일어나게 할 것이며 애니메이션이 변경된다면 이는 해제되도록 할 것이다
		list<pair<int32, Vector3>>									m_lstBoneRotation;

		// 재생 카운트
		int32														m_iNumUpperCurrentAnimLoop = 0;
		int32														m_iNumDownCurrentAnimLoop = 0;

		// 툴용 옵션
		bool														m_bStopAnimation = false;

		// 이름과 인덱스 매칭용 umap
		unordered_map<wstring, int32>								m_umapAnimatoinNameID;

		// 상체 쉐이킹 용
		int32														m_iSpineBoneIndex = -1;

		bool														m_bShakeUpperBody = false;
		float														m_fShakePower = 0.f;
		float														m_fShakeTime = 0.f;
		float														m_fShakeCheck = 0.f;
	};
}