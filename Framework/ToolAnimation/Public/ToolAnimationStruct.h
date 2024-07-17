#pragma once

namespace ToolAnimation
{
	// 충돌 이벤트
	struct FCollisionOffset
	{
		float m_fSphereRadius = 0.f;
		Vector3 m_vRelativeLocation;
	};

	struct FToolCollisionNotify
	{
		string m_strAttachBoneName = "";
		
		list<FCollisionOffset> m_lstOffsetData;
	};

	// 사운드 이벤트
	struct FToolSoundNotify
	{
		string m_strSoundID = "";
	};

	// 이펙트 이벤트
	struct FToolEffectNotify
	{
		bool m_bFollowBone = false;

		string m_strAttachBoneName = "";
		string m_strEffectName = "";

		Vector3 m_vOffset;
	};

	// 노티파이 데이터
	struct FToolNotifyData
	{
		int32 iNotifyID = 0;

		string m_strNotifyType = "";

		float m_fStartTrackPos = 0;
		float m_fEndTrackPos = 0;

		FToolCollisionNotify m_tCollisionNotify;
		FToolSoundNotify m_tSoundNotify;
		FToolEffectNotify m_tEffectNotify;
		string m_strFunctionNotify = "";
	};
}