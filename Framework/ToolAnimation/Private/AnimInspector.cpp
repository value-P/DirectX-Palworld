#include "pch.h"
#include "AnimInspector.h"

// Engine Singleton & Static Class
#include "EngineModule.h"
#include "CameraManager.h"
#include "StringUtility.h"
#include "MathUtility.h"
#include "SoundManager.h"

// Engine Class
#include "MeshComponent.h"
#include "Model.h"
#include "Bone.h"
#include "Level.h"

// Tool Singleton
#include "ModelBlackBoard.h"
#include "EffectManager.h"

// Tool Class
#include "SphereObject.h"
#include "ToolAnimObject.h"

void ToolAnimation::CAnimInspector::Initialize()
{
	CWindowBase::Initialize();

	m_wpCameraManager = CEngineModule::GetInstance()->GetCameraManager();
	m_wpEffectManager = CEffectManager::GetInstance();

	CSphereObject::FGameObjectDesc tObjDesc;
	tObjDesc.m_bActive = false;
	m_spSphere = CSphereObject::Create(make_shared<CSphereObject::FGameObjectDesc>(tObjDesc));
	m_wpLevel.lock()->AddGameObject(m_spSphere);

	m_spEffectPointSphere = CSphereObject::Create(make_shared<CSphereObject::FGameObjectDesc>(tObjDesc));
	m_wpLevel.lock()->AddGameObject(m_spEffectPointSphere);
	m_spEffectPointSphere->SetRadius(0.1f);
}

void ToolAnimation::CAnimInspector::Tick(float _fDeltaSeconds)
{
	CWindowBase::Tick(_fDeltaSeconds);

	ComputeSphereWithBone();

	if (ImGui::Begin("Inspector", NULL, m_eWindowFlag))
	{
		if (ImGui::BeginTabBar("Model", ImGuiTabBarFlags_None))
		{
			ModelTab();
			NotifyTab();

			ImGui::EndTabBar();
		}
	}
	
	ImGui::End();
}

void ToolAnimation::CAnimInspector::Release()
{
	CWindowBase::Release();

	if (m_spSphere)
	{
		m_spSphere = nullptr;
	}

	if (m_spEffectPointSphere)
	{
		m_spEffectPointSphere = nullptr;
	}

	if (m_spSelectedBone)
	{
		m_spSelectedBone = nullptr;
	}
}

void ToolAnimation::CAnimInspector::ModelTab()
{
	if (ImGui::BeginTabItem("Model Select"))
	{
		ModelList();

		ImGui::Separator();

		AnimationList();

		ImGui::Separator();

		SaveButton();

		ImGui::EndTabItem();
	}
}

void ToolAnimation::CAnimInspector::ModelList()
{
	if (ImGui::BeginListBox("AnimModels"))
	{
		for (size_t iModelIndex = 0; iModelIndex < m_wpModelBlackBoard.lock()->GetObjectNames().size(); ++iModelIndex)
		{
			if (ImGui::Selectable(m_wpModelBlackBoard.lock()->GetObjectNames()[iModelIndex].c_str(), true))
				m_iModelIndex = (int32)iModelIndex;
		}

		ImGui::EndListBox();
	}

	if (ImGui::Button("Change"))
	{
		if (m_iModelIndex < 0 || m_iModelIndex >= m_wpModelBlackBoard.lock()->GetObjectNames().size())
			return;

		string strModelName = m_wpModelBlackBoard.lock()->GetObjectNames()[m_iModelIndex];
		m_wpModelBlackBoard.lock()->ChangeMainObject(strModelName);
	}
}
void ToolAnimation::CAnimInspector::AnimationList()
{
	auto pAnimationNames = m_wpModelBlackBoard.lock()->GetCurrentAnimations();
	if (pAnimationNames == nullptr) { return; }

	if (ImGui::BeginListBox("Animations"))
	{
		for (size_t iAnimationIndex = 0; iAnimationIndex < (*pAnimationNames).size(); ++iAnimationIndex)
		{
			if (ImGui::Selectable((*pAnimationNames)[iAnimationIndex].c_str(), true))
				m_iAnimationIndex = (int32)iAnimationIndex;
		}

		ImGui::EndListBox();
	}

	// 그냥 박스에서 선택
	if (ImGui::Button("Change Animation"))
	{
		if (m_iAnimationIndex < 0 || m_iAnimationIndex >= (*pAnimationNames).size())
			return;

		wstring strModelName = CStringUtility::ToWideString((*pAnimationNames)[m_iAnimationIndex]);
		m_wpModelBlackBoard.lock()->GetMainObject()->GetMeshComponent()->ChangeAnimation(strModelName);
	}

	// 애니메이션 이름으로 변경
	ImGui::InputText("Animation Name", szAnimName, MAX_PATH);

	if (ImGui::Button("Change With Name"))
	{
		string strName = szAnimName;
		m_wpModelBlackBoard.lock()->GetMainObject()->GetMeshComponent()->ChangeAnimation(CStringUtility::ToWideString(strName));
	}
}

void ToolAnimation::CAnimInspector::SaveButton()
{
	ImGui::Dummy(ImVec2(10.f, 50.f));
	
	ImGui::Dummy(ImVec2(30.f, 10.f));
	ImGui::SameLine();
	if (ImGui::Button("Save current Model Notify",ImVec2(180.f,25.f)))
	{
		m_wpModelBlackBoard.lock()->SaveCurrentModelNotify();
	}

	ImGui::Dummy(ImVec2(10.f, 10.f));

	ImGui::Dummy(ImVec2(30.f, 10.f));
	ImGui::SameLine();
	if (ImGui::Button("Save Entire Model Notify", ImVec2(180.f, 25.f)))
	{
		m_wpModelBlackBoard.lock()->SaveAllModelNotifyData();
	}
}

void ToolAnimation::CAnimInspector::NotifyTab()
{
	if (ImGui::BeginTabItem("Notify"))
	{
		NotifyList();

		ImGui::Separator();

		ImGui::EndTabItem();
	}
}
void ToolAnimation::CAnimInspector::NotifyList()
{
	if (ImGui::BeginCombo("Notify Type", m_arrNotifyList[m_iCurrentNotifyType].c_str()))
	{
		for (int iNotifyIndex = 0; iNotifyIndex < m_arrNotifyList.size(); ++iNotifyIndex)
		{
			if (ImGui::Selectable(m_arrNotifyList[iNotifyIndex].c_str(), true))
				m_iCurrentNotifyType = iNotifyIndex;
		}

		ImGui::EndCombo();
	}

	switch (m_iCurrentNotifyType)
	{
	case 0:
		CollisoinNotify();
		m_spSphere->SetActive(true);
		m_spEffectPointSphere->SetActive(false);
		break;
	case 1:
		SoundNotify();
		break;
	case 2:
		EffectNotify();
		m_spEffectPointSphere->SetActive(true);
		m_spSphere->SetActive(false);
		break;
	case 3:
		FunctionNotify();
		break;
	default:
		m_spSphere->SetActive(false);
		m_spEffectPointSphere->SetActive(false);
		break;
	}
}

void ToolAnimation::CAnimInspector::CollisoinNotify()
{
	BoneList();

	ImGui::Separator();

	ColliderSetting();

	if (ImGui::Button("Add Notify : col"))
	{
		m_wpModelBlackBoard.lock()->AddCollisionNotify(m_strSelectedBoneName, m_tCollisionOffset, m_fEndTrackPos);
	}
}
void ToolAnimation::CAnimInspector::BoneList()
{
	ImGui::Text("Bone Hierarchy");

	shared_ptr<CToolAnimObject> spMainObj = m_wpModelBlackBoard.lock()->GetMainObject();
	if (spMainObj == nullptr) { return; }

	shared_ptr<CBone> spRootBone = spMainObj->GetMeshComponent()->GetBonePtr(L"root");

	BoneTreeNode(spRootBone, spMainObj->GetMeshComponent());
}
void ToolAnimation::CAnimInspector::ColliderSetting()
{
	m_tCollisionOffset.m_vRelativeLocation = m_spSphere->GetLocalPos();
	m_tCollisionOffset.m_fSphereRadius = m_spSphere->GetRadius();

	ImGui::Text(m_strSelectedBoneName.c_str());
	ImGui::DragFloat("Raidus", &m_tCollisionOffset.m_fSphereRadius, 0.01f, -10.f, 10.f, "%.2f");
	ImGui::DragFloat3("Local Position", (float*)&m_tCollisionOffset.m_vRelativeLocation, 0.01f, -10.f, 10.f, "%.2f");

	m_spSphere->SetLocalPos(m_tCollisionOffset.m_vRelativeLocation);
	m_spSphere->SetRadius(m_tCollisionOffset.m_fSphereRadius);

	m_fEndTrackPos = max(m_fEndTrackPos, m_tAnimData.m_fTrackPosition);

	ImGui::DragFloat("End TrackPos", &m_fEndTrackPos, 0.01f, m_tAnimData.m_fTrackPosition, m_tAnimData.m_fDuration);
}

void ToolAnimation::CAnimInspector::SoundNotify()
{
	SoundSetting();

	ImGui::Separator();

	ImGui::Dummy(ImVec2(10.f, 10.f));

	if (ImGui::Button("Add Notify : Sound"))
	{
		m_wpModelBlackBoard.lock()->AddSoundNotify(m_tSoundNotify);
	}
}
void ToolAnimation::CAnimInspector::SoundSetting()
{
	if (ImGui::InputText("Sound ID", szSoundID, MAX_PATH))
	{
		m_tSoundNotify.m_strSoundID = szSoundID;
	}

	if (ImGui::Button("PlaySound"))
	{
		CEngineModule::GetInstance()->GetSoundManager()->PlaySoundEx(m_tSoundNotify.m_strSoundID);
	}
}

void ToolAnimation::CAnimInspector::EffectNotify()
{
	BoneList();

	ImGui::Separator();

	EffectList();

	ImGui::Separator();

	EffectSetting();

	ImGui::Separator();

	ImGui::Dummy(ImVec2(10.f, 10.f));

	if (ImGui::Button("Add Notify : fx"))
	{
		if (m_tEffectNotify.m_strAttachBoneName == "" || m_tEffectNotify.m_strEffectName == "") { return; }

		m_wpModelBlackBoard.lock()->AddEffectNotify(m_tEffectNotify);
	}
}
void ToolAnimation::CAnimInspector::EffectList()
{
	ImGui::Text("Effect List");

	if (m_strSelectedBoneName == "") { return; }

	shared_ptr<CToolAnimObject> spMainObj = m_wpModelBlackBoard.lock()->GetMainObject();
	if (spMainObj == nullptr) { return; }

	if (ImGui::BeginListBox("Effect Names"))
	{
		for (auto& strName : m_wpEffectManager.lock()->GetEffectNames())
		{
			if (ImGui::Selectable(strName.c_str(), true))
				m_tEffectNotify.m_strEffectName = strName;
		}

		ImGui::EndListBox();
	}
}
void ToolAnimation::CAnimInspector::EffectSetting()
{
	if (m_tEffectNotify.m_strEffectName == "") { return; }

	m_tEffectNotify.m_strAttachBoneName = m_strSelectedBoneName;

	ImGui::Text(m_strSelectedBoneName.c_str());
	if (ImGui::DragFloat3("Local Position", (float*)&m_tEffectNotify.m_vOffset, 0.01f, -100.f, 100.f, "%.2f"))
	{
		ComputeSphereWithBone();
	}
	
	if (ImGui::Checkbox("Use FollowBone", &m_tEffectNotify.m_bFollowBone))
	{
		ComputeSphereWithBone();
	}

	if (ImGui::Button("Play Effect"))
	{
		shared_ptr<CGameObject> spObject = nullptr;
		if (nullptr != m_wpModelBlackBoard.lock()->GetMainObject() &&
			m_tEffectNotify.m_bFollowBone)
		{
			spObject = m_wpModelBlackBoard.lock()->GetMainObject();
			m_wpEffectManager.lock()->ActivateEffectCombined(m_tEffectNotify.m_strEffectName, spObject, m_spSelectedBone, m_tEffectNotify.m_vOffset);
		}
		else
		{
			m_wpEffectManager.lock()->ActivateEffect(m_tEffectNotify.m_strEffectName, m_spEffectPointSphere->GetPosition().value());
		}
		
	}
}

void ToolAnimation::CAnimInspector::FunctionNotify()
{
	ImGui::InputText("EffectNotify", m_szFunctionName, sizeof(m_szFunctionName[0]) * IM_ARRAYSIZE(m_szFunctionName));

	ImGui::Separator();

	ImGui::Dummy(ImVec2(10.f, 10.f));

	if (ImGui::Button("Add Notify : Function"))
	{
		string strNotify = m_szFunctionName;

		if (strNotify == "") { return; }

		m_wpModelBlackBoard.lock()->AddFunctionNotify(strNotify);
	}
}

void ToolAnimation::CAnimInspector::BoneTreeNode(shared_ptr<CBone> _spBone, shared_ptr<CMeshComponent> _spMeshComponent)
{
	string strBoneName = CStringUtility::ToString(_spBone->GetBoneName());

	ImGuiTreeNodeFlags iNodeFlag;

	if (_spBone->GetChildBoneIndices().size() <= 0)
		iNodeFlag = ImGuiTreeNodeFlags_Selected| ImGuiTreeNodeFlags_Leaf;
	else
		iNodeFlag = ImGuiTreeNodeFlags_Selected;

	if (ImGui::TreeNodeEx(strBoneName.c_str(), iNodeFlag))
	{
		if (ImGui::IsItemClicked()) 
		{
			m_spSelectedBone = _spBone; 
			m_strSelectedBoneName = CStringUtility::ToString(m_spSelectedBone->GetBoneName());
		}

		for (int32 iChildBoneIdx : _spBone->GetChildBoneIndices())
		{
			BoneTreeNode(_spMeshComponent->GetBonePtr(iChildBoneIdx), _spMeshComponent);
		}

		ImGui::TreePop();
	}
}

void ToolAnimation::CAnimInspector::ComputeSphereWithBone()
{
	if (!m_spSelectedBone) { return; }

	// 콜라이더 스피어
	if (m_arrNotifyList[m_iCurrentNotifyType] == "Collision")
	{
		Matrix matCombined = m_spSelectedBone->GetCombinedTransformationMatrix();
		m_spSphere->SetTransformMatrix(matCombined);
		m_spSphere->SetScale(Vector3(1.f, 1.f, 1.f));
	}
	// 이펙트 스피어
	else if (m_arrNotifyList[m_iCurrentNotifyType] == "Effect")
	{
		if (m_tEffectNotify.m_bFollowBone)
		{
			Matrix matCombined = m_spSelectedBone->GetCombinedTransformationMatrix();
			m_spEffectPointSphere->SetPosition(matCombined.Translation());
		}
		else
		{
			Matrix matLocal = Matrix::CreateTranslation(m_tEffectNotify.m_vOffset);
			Matrix matCombined = CMathUtility::GetScaledKilledMatrix(m_spSelectedBone->GetCombinedTransformationMatrix());
			Matrix matWorld = matLocal * matCombined;
			
			m_spEffectPointSphere->SetPosition(matWorld.Translation());
		}
	}
}
