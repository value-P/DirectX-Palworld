#pragma once

namespace ToolAnimation
{
	// �浹 �̺�Ʈ
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

	// ���� �̺�Ʈ
	struct FToolSoundNotify
	{
		string m_strSoundID = "";
	};

	// ����Ʈ �̺�Ʈ
	struct FToolEffectNotify
	{
		bool m_bFollowBone = false;

		string m_strAttachBoneName = "";
		string m_strEffectName = "";

		Vector3 m_vOffset;
	};

	// ��Ƽ���� ������
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