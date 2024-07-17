#pragma once
#include "WindowBase.h"

namespace Engine
{
	class CCameraManager;
	class CMeshComponent;
	class CBone;
}

namespace ToolAnimation
{
	class CSphereObject;
	class CEffectManager;

    class CAnimInspector : public CWindowBase
    {
    /********************
		Ctor/Dtor
	********************/
	public:
		explicit CAnimInspector() = default;
		virtual ~CAnimInspector() = default;

	/********************
		Events
	********************/
	public:
		virtual void Initialize() override;
		virtual void Tick(float _fDeltaSeconds) override;
		virtual void Release() override;

	/********************
		Methods
	********************/
	public:
		void ModelTab();
			void ModelList();
			void AnimationList();
			void SaveButton();

		void NotifyTab();
			void NotifyList();
				void CollisoinNotify();
					void BoneList();
					void ColliderSetting();
				void SoundNotify();
					void SoundSetting();
				void EffectNotify();
					void EffectList();
					void EffectSetting();
				void FunctionNotify();

	private:
		void BoneTreeNode(shared_ptr<CBone> _spBone, shared_ptr<CMeshComponent> _spMeshComponent);
		void ComputeSphereWithBone();

	/********************
		Getter/Setter
	********************/
	public:

	/********************
		Data Members
	********************/
	private:
		// Default
		weak_ptr<CCameraManager> m_wpCameraManager;
		weak_ptr<CEffectManager> m_wpEffectManager;

		// Model List
		int32 m_iModelIndex = 0;
		// Animation List
		int32 m_iAnimationIndex = 0;

		// Notify List
		array<string, 4> m_arrNotifyList = {"Collision","Sound","Effect","Function"};
		int32 m_iCurrentNotifyType = 0;

		// Notify

		// Bone List
		shared_ptr<CBone> m_spSelectedBone = nullptr;
		string m_strSelectedBoneName = "";
		
		// Collision Notify
		FCollisionOffset m_tCollisionOffset;
		float m_fEndTrackPos = 0;
		
		// Sound Notify
		char szAnimName[MAX_PATH] = "";
		char szSoundID[MAX_PATH] = "";
		FToolSoundNotify m_tSoundNotify;

		// EffectNotify
		FToolEffectNotify m_tEffectNotify;

		// FunctionNotify
		char m_szFunctionName[MAX_PATH];

		// Sphere
		shared_ptr<CSphereObject> m_spSphere = nullptr;
		shared_ptr<CSphereObject> m_spEffectPointSphere = nullptr;
    };
}


