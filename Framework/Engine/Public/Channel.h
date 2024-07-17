#pragma once

namespace Engine
{
	class CBone;
	class CModel;

	class CChannel final
	{
		/********************
			Struct
		********************/
	public:
		struct KEYFRAME
		{
			Vector3 vScale, vPosition;
			Quaternion qRotation;
			float fTime = 0;
		};

		/********************
			Ctor/Dtor
		********************/
	public:
		CChannel() = default;
		virtual ~CChannel() = default;

		/********************
			Get
		********************/
	public:
		KEYFRAME	GetCurrentKeyFrame() { return m_tCurrentKeyFrame; }
		KEYFRAME	GetStartKeyFrame() { return m_vecKeyFrames[0]; }
		int32		GetNumKeyFrame() { return (int32)m_vecKeyFrames.size(); }
		int32		GetBoneIndex() { return m_iBoneIndex; }

		/********************
			Set
		********************/
	public:
		void		SetLerpTime(float _fLerpTime) { m_fLerpTime = _fLerpTime; }
		void		ResetLerpChecker() { m_fLerpChecker = 0.f; }
		void		SetPrevTranslationToStart() { m_vPrevTranslation = m_vecKeyFrames.front().vPosition; }
		void		SetPrevTranslationToEnd() { m_vPrevTranslation = m_vecKeyFrames.back().vPosition; }

		/********************
			Events
		********************/
	public:
		static std::shared_ptr<CChannel> Create(ifstream& _ifFile, shared_ptr<CModel> _spModel);
		virtual void Initialize(ifstream& _ifFile, shared_ptr<CModel> _spModel);
		void Release();

		/********************
			Methods
		********************/
	public:
		void PlayKeyFrame(float _fTrackPosition, const vector<std::shared_ptr<CBone>>& _vecBones, Vector3& _vRootMotionVelocity);
		bool LerpToAnotherAnimKeyframe(float _fAddTime, KEYFRAME _tNextKeyFrame, const vector<std::shared_ptr<CBone>>& _vecBones);
		bool LerpToStartKeyFrame(float _fAddTime, const vector<std::shared_ptr<CBone>>& _vecBones);

		/********************
			Data Members
		********************/
	private:
		wstring				m_wstrChannelName = L"";

		float				m_fLerpChecker = 0.f;
		float				m_fDefaultLerpTime = 0.2f;
		float				m_fLerpTime = 0.2f; // lerp에 사용되는 시간값
		int32				m_iBoneIndex = 0;	// bone의 인덱스

		int32				m_iNumKeyFrames = 0;
		vector<KEYFRAME>	m_vecKeyFrames;
		KEYFRAME			m_tCurrentKeyFrame;

		// 루트모션 계산용
		Vector3				m_vPrevTranslation = Vector3(0.f, 0.f, 0.f);
	};
}