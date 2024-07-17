#pragma once
#include "Manager.h"

namespace Engine
{
	class CLevel;
}

namespace ToolAnimation
{
	using KeyFrameNotify = unordered_map<int32, list<FToolNotifyData>>;
	using AnimationNotify = unordered_map<string, KeyFrameNotify>;
	
	class CToolAnimObject;
    
	enum class ENotifyType
	{
		None,
		Collision,
		Sound,
		Effect,
	};

	class CModelBlackBoard : public CManager
    {
        DECLARE_SINGLETON(CModelBlackBoard)

	public:
		void SetCurrentLevel(shared_ptr<CLevel> _spCurrentLevel);
		shared_ptr<CToolAnimObject> GetMainObject() { return m_spMainObject; }

		const vector<string>& GetObjectNames() { return m_vecObjectNames; }
		const vector<string>* GetCurrentAnimations();

		optional<FAnimationData> GetCurrentModelAnimData();

	public:
		HRESULT Initialize();
		void Release();

	private:
		void LoadModelFile();
		void InsertNotifys(const string& _strObjName, const string& _strAnimationName);

	public:
		void ChangeMainObject(const string& _strObjectName);

		// Notify 데이터 Json파싱
		void SaveAllModelNotifyData();

		void SaveCurrentModelNotify();

		// 컨테이너 채우기
		bool EmplaceNotify(const FAnimationData& _tAnimData);

		// 노티파이 정보 삽입
		void AddCollisionNotify(const string& _strAttachBoneName, const FCollisionOffset& _tCollisionOffset, float _fEndTrackPos);

		void AddSoundNotify(const FToolSoundNotify& _tSoundNotify);

		void AddEffectNotify(const FToolEffectNotify& _tEffectNotify);

		void AddFunctionNotify(const string& _strFunctionName);

		// 노티파이 정보 삭제
		void DeleteNotify(int32 _iNotifyIndex);

		// 노티파이 정보 읽어오기
		list<FToolNotifyData>* GetCurrentKeyFrameNotify();

		// 애니메이션 on / off
		void ToggleAnimation();

		void StopAnimation();

		void ResumeAnimation();

	private:
		void SaveModelNotify(const string& _strModelName, bool _bAnnounce = true);

	private:
		// 로드용 데이터
		weak_ptr<CLevel> m_wpCurrentLevel;
		string m_strFilePath = "../../Resource/Models/Dat/AnimationToolModel";
		Matrix m_matPivot;

		// 로드된 오브젝트들
		unordered_map<string, shared_ptr<CToolAnimObject>> m_umapAnimObject;
		unordered_map<string, vector<string>> m_umapObjectAnimationNames;
		vector<string> m_vecObjectNames;

		// 현재 선택한 오브젝트
		shared_ptr<CToolAnimObject> m_spMainObject = nullptr;
		string m_strCurrentMainObjectName = "";

		// Notify 데이터
		unordered_map<string, AnimationNotify> m_umapModelsNotify;

		// 애니메이션 토글용 데이터
		bool m_bStopAnimation = false;

		ENotifyType m_eCurrentNotifyType = ENotifyType::None;
	};
}


