#pragma once

namespace Engine
{
	class CModel;
	class CChannel;
	class CBone;

	class CAnimation final
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		CAnimation() = default;
		CAnimation(const CAnimation& rhs);
		virtual ~CAnimation() = default;
		
	/********************
		Getter/Setter
	********************/
	public:
		const unordered_map<uint32, shared_ptr<CChannel>>& GetChannels() { return m_umapChannels; }

		float GetTrackPosition() { return m_fTrackPosition; }

		float GetTickPerSecond() { return m_fTickPerSec; }

		float GetDuration() { return m_fDuration; }

		float GetCurrentFrameRatio() { return m_fTrackPosition / m_fDuration; }

		list<FAnimNotifyData> GetCurrentFrameNotifyData();

		Vector3 GetRootMotionVelocity() { return m_vRootMotionVelocity; }

		int32 GetGroupID() { return m_iAnimationGroupID; }

		wstring GetAnimationName() { return m_wstrAnimationName; }

		void MoveTrackPosition(float _fTrackPos, const list<int32>& _lstBoneIndices);

		void SetTrackPosition(float _fTrackPos) { m_fTrackPosition = _fTrackPos; }

		void SetTrackPositionWithRatio(float _fTrackPosRatio) { m_fTrackPosition = m_fDuration * _fTrackPosRatio; }

		void SetLerpTime(float _fLerpTime, const list<int32>& _lstBoneIndices);

		void ResetLerpChecker(const list<int32>& _lstBoneIndiecse);

		void SetAnimGroup(const wstring& _wstrGroupName, int32 _iGroupID) { m_wstrGroupName = _wstrGroupName; m_iAnimationGroupID = _iGroupID; }

		void SetAnimationID(int32 _iID) { m_iAnimationID = _iID; }

		bool IsSameGroup(shared_ptr<CAnimation> _spOtherAnimation);

	/********************
		Events
	********************/
	public:
		static shared_ptr<CAnimation>	Create(ifstream& _ifFile, shared_ptr<CModel> _spModel);

		virtual void					Initialize(ifstream& _ifFile, shared_ptr<CModel> _spModel);
			
		void Release();

	private:
		void InitAnimNotifyData();

		void FindRootBoneIndex();
	/********************
		Methods
	********************/
	public:
		void ResetChannel();

		bool PlayAnimation(float _fDeltaSeconds, bool _bUseReversePlay, const list<int32>& _lstBoneIndices, float _fLoopStartRatio);

		void PlayAnimationInLerp(float _fDeltaSeconds, bool _bUseReversePlay, const list<int32>& _lstBoneIndices);

		bool ChangeAnimationLerp(float _fDeltaSeconds, const shared_ptr<CAnimation> _spNextAnim, bool _bUseReversePlay, const list<int32>& _lstBoneIndices);
		
	private:
		void ResetNotifyActivated();

		void PlayNotify(bool _bPlayReverse);

	/********************
		Data Members
	********************/
	private:
		float										m_fDuration				= 0.f;
		float										m_fTickPerSec			= 0.f;

		float										m_fTrackPosition		= 0.f;

		bool										m_bFinish				= false;

		wstring										m_wstrAnimationName = L"";
		wstring										m_wstrGroupName = L"";

		int32										m_iAnimationID = -1;
		int32										m_iAnimationGroupID = -1;

		int32										m_iRootChannelIndex = 0;

		Vector3										m_vRootMotionVelocity = Vector3(0.f, 0.f, 0.f);

		weak_ptr<CModel>								m_wpOwnerModel;			// 해당 Animation을 소유하고 있는 Model class 에 대한 주소
		vector<shared_ptr<CBone>>						m_vecBonesRef;			// 모델의 뼈 컨테이너에 대한 참조값
		unordered_map<uint32, shared_ptr<CChannel>>		m_umapChannels;			// key : 채널과 매칭되는 본 인덱스,	value : 채널

		list<pair<FAnimNotifyData,bool>>				m_lstAnimNotifyData;	// 애니메이션 노티파이/ 노티파이가 현재 루프에서 실행됬다면 true
		list<FAnimNotifyData>							m_lstActivatedNotifies; // 현재 프레임에서 실행된 노티파이들의 집합
	};
}