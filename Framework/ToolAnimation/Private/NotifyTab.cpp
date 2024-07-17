#include "pch.h"
#include "NotifyTab.h"

#include "EngineModule.h"
#include "SoundManager.h"
#include "MathUtility.h"
#include "EffectManager.h"
#include "ModelBlackBoard.h"
#include "ToolAnimObject.h"
#include "MeshComponent.h"
#include "Model.h"
#include "Bone.h"
#include "StringUtility.h"

void ToolAnimation::CNotifyTab::Initialize()
{
	CWindowBase::Initialize();
}

void ToolAnimation::CNotifyTab::Tick(float _fDeltaSeconds)
{
	if (ImGui::Begin("Notify", NULL, m_eWindowFlag))
	{
		ImGui::PushItemWidth(190);

		NotifyTab();

		ImGui::End();
	}

	if (ImGui::Begin("Notify Detail", NULL, m_eWindowFlag))
	{
		ImGui::PushItemWidth(190);

		NotifyDetail();

		ImGui::End();
	}

	PlayNotify();
}

void ToolAnimation::CNotifyTab::Release()
{
	CWindowBase::Release();
}

void ToolAnimation::CNotifyTab::NotifyTab()
{
	NotifyList();

	ImGui::Separator();

	NotifyDelete();
}

void ToolAnimation::CNotifyTab::NotifyList()
{
	if (!m_wpModelBlackBoard.lock()->GetCurrentKeyFrameNotify()) 
	{
		m_tNotifyData.m_strNotifyType = "";
		return; 
	}

	list<FToolNotifyData> lstNotifyData = *m_wpModelBlackBoard.lock()->GetCurrentKeyFrameNotify();

	if (ImGui::BeginListBox("Notify Lists"))
	{
		int iCnt = 0;
		for (auto& tNotifyDat : lstNotifyData)
		{
			string notifyName = to_string(iCnt) + " . " + tNotifyDat.m_strNotifyType;
			if (ImGui::Selectable(notifyName.c_str()))
			{
				m_tNotifyData = tNotifyDat;
				m_iSelectedNotifyIndex = iCnt;
			}

			++iCnt;
		}
		
		ImGui::EndListBox();
	}
}

void ToolAnimation::CNotifyTab::NotifyDelete()
{
	if (ImGui::Button("DelteNotify"))
	{
		m_wpModelBlackBoard.lock()->DeleteNotify(m_iSelectedNotifyIndex);
	}
}

void ToolAnimation::CNotifyTab::NotifyDetail()
{
	if (m_tNotifyData.m_strNotifyType == "") { return; }

	ImGui::BulletText("Type : "); ImGui::SameLine(); ImGui::Text(m_tNotifyData.m_strNotifyType.c_str());

	if (m_tNotifyData.m_strNotifyType == "Collision")
	{
		ImGui::BulletText("Attach Bone Name : "); ImGui::SameLine(); ImGui::Text(m_tNotifyData.m_tCollisionNotify.m_strAttachBoneName.c_str());
		ImGui::BulletText("End KeyFrame : "); ImGui::SameLine(); ImGui::InputFloat("End KeyFrame", &m_tNotifyData.m_fEndTrackPos);
		
		if (ImGui::TreeNode("Collision Offset Data"))
		{
			int32 iCnt = 0;
			for (auto& tCollisionOffset : m_tNotifyData.m_tCollisionNotify.m_lstOffsetData)
			{
				string strLabel = "Offset Data" + to_string(iCnt);
				if (ImGui::TreeNode(strLabel.c_str()))
				{
					ImGui::BulletText("Sphere Radius : "); 
					ImGui::SameLine(); 
					ImGui::InputFloat("radius", &tCollisionOffset.m_fSphereRadius);

					ImGui::BulletText("Offset X: ");
					ImGui::SameLine(); 
					ImGui::InputFloat("locationX", &tCollisionOffset.m_vRelativeLocation.x);
					
					ImGui::BulletText("Offset Y: ");
					ImGui::SameLine(); 
					ImGui::InputFloat("locationY", &tCollisionOffset.m_vRelativeLocation.y);
					
					ImGui::BulletText("Offset Z: ");
					ImGui::SameLine(); 
					ImGui::InputFloat("locationZ", &tCollisionOffset.m_vRelativeLocation.z);

					ImGui::TreePop();
				}

				++iCnt;
			}

			ImGui::TreePop();
		}
	}
	else if (m_tNotifyData.m_strNotifyType == "Sound")
	{
		ImGui::BulletText("Sound ID : ");
		ImGui::SameLine();
		ImGui::Text(m_tNotifyData.m_tSoundNotify.m_strSoundID.c_str());

		if (ImGui::Button("Play Sound Notify"))
		{
			CEngineModule::GetInstance()->GetSoundManager()->PlaySoundEx(m_tNotifyData.m_tSoundNotify.m_strSoundID);
		}
	}
	else if (m_tNotifyData.m_strNotifyType == "Effect")
	{
		ImGui::BulletText("Attach Bone Name : "); ImGui::SameLine(); ImGui::Text(m_tNotifyData.m_tEffectNotify.m_strAttachBoneName.c_str());
		ImGui::BulletText("Effect Name : "); ImGui::SameLine(); ImGui::Text(m_tNotifyData.m_tEffectNotify.m_strEffectName.c_str());
		
		ImGui::BulletText("Offset X: ");
		ImGui::SameLine();
		ImGui::InputFloat("locationX", &m_tNotifyData.m_tEffectNotify.m_vOffset.x);

		ImGui::BulletText("Offset Y: ");
		ImGui::SameLine();
		ImGui::InputFloat("locationY", &m_tNotifyData.m_tEffectNotify.m_vOffset.y);

		ImGui::BulletText("Offset Z: ");
		ImGui::SameLine();
		ImGui::InputFloat("locationZ", &m_tNotifyData.m_tEffectNotify.m_vOffset.z);
	}
	else if (m_tNotifyData.m_strNotifyType == "Function")
	{
		ImGui::BulletText("FunctionName : ");
		ImGui::SameLine();
		ImGui::Text(m_tNotifyData.m_strFunctionNotify.c_str());
	}
}

void ToolAnimation::CNotifyTab::PlayNotify()
{
	shared_ptr<CToolAnimObject> spObject = m_wpModelBlackBoard.lock()->GetMainObject();
	FAnimationData BaseAnimData;

	if (spObject)
	{
		BaseAnimData = spObject->GetMeshComponent()->GetModelAnimationData().m_tBaseAnimData;
		if (BaseAnimData.m_bLoopEnd)
		{
			m_lstPlayedNotifyID.clear();
		}
	}
	else { return; }

	if (!m_wpModelBlackBoard.lock()->GetCurrentKeyFrameNotify()) { return; }

	list<FToolNotifyData> lstNotifyData = *m_wpModelBlackBoard.lock()->GetCurrentKeyFrameNotify();

	for (auto& tNotifyDat : lstNotifyData)
	{
		auto iter = find_if(m_lstPlayedNotifyID.begin(), m_lstPlayedNotifyID.end(), [&tNotifyDat](int32 _iPlayedID)->bool {return tNotifyDat.iNotifyID == _iPlayedID; });
		if (iter != m_lstPlayedNotifyID.end()) { continue; }

		if (tNotifyDat.m_fStartTrackPos >= BaseAnimData.m_fTrackPosition)
		{
			if (tNotifyDat.m_strNotifyType == "Effect")
			{
				shared_ptr<CBone> spBone = spObject->GetMeshComponent()->GetBonePtr(CStringUtility::ToWideString(tNotifyDat.m_tEffectNotify.m_strAttachBoneName));
				Matrix matBone = CMathUtility::GetScaledKilledMatrix(spBone->GetCombinedTransformationMatrix());
				Matrix matRelative = Matrix::CreateTranslation(tNotifyDat.m_tEffectNotify.m_vOffset);
				Matrix matWorld = matRelative * matBone;

				Vector3 vPos = matRelative.Translation();

				if (m_tNotifyData.m_tEffectNotify.m_bFollowBone)
				{
					CEffectManager::GetInstance()->ActivateEffectCombined(tNotifyDat.m_tEffectNotify.m_strEffectName, spObject, spBone, tNotifyDat.m_tEffectNotify.m_vOffset);
				}
				else
				{
					CEffectManager::GetInstance()->ActivateEffect(tNotifyDat.m_tEffectNotify.m_strEffectName, vPos);
				}

				m_lstPlayedNotifyID.push_back(tNotifyDat.iNotifyID);
			}
		}
	}
}
