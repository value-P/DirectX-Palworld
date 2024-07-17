#include "pch.h"
#include "Sequencer.h"

#include "EngineModule.h"
#include "InputManager.h"

#include "ModelBlackBoard.h"
#include "ToolAnimObject.h"
#include "MeshComponent.h"
#include "Model.h"

void ToolAnimation::CSequencer::Initialize()
{
    CWindowBase::Initialize();
}

void ToolAnimation::CSequencer::Tick(float _fDeltaSeconds)
{
    CWindowBase::Tick(_fDeltaSeconds);

    if (CEngineModule::GetInstance()->GetInputManager()->IsKeyDown(DIK_SPACE))
    {
        m_wpModelBlackBoard.lock()->ToggleAnimation();
    }

	if (ImGui::Begin("Sequence", NULL, m_eWindowFlag | ImGuiWindowFlags_AlwaysAutoResize))
	{
        ImGui::PushItemWidth(650);

		SequenceTab();

		ImGui::End();
	}
}

void ToolAnimation::CSequencer::Release()
{
    CWindowBase::Release();
}

void ToolAnimation::CSequencer::SequenceTab()
{
    int32 minKeyFrame = 0;
    if (ImGui::SliderFloat("TrackPosition",&m_tAnimData.m_fTrackPosition, 0, m_tAnimData.m_fDuration))
    {
        m_wpModelBlackBoard.lock()->GetMainObject()->GetMeshComponent()->SetTrackPosition(m_tAnimData.m_fTrackPosition);
    }

    ImGui::Dummy(ImVec2(10.f, 10.f));

    if (ImGui::SliderFloat("AnimSpeed", &m_fAnimationSpeed, 0.f, 1.f))
    {
        g_fTimeScale = m_fAnimationSpeed;
    }

    ImGui::Dummy(ImVec2(10.f, 10.f));

    if (ImGui::Button("Stop"))
    {
        m_wpModelBlackBoard.lock()->StopAnimation();
    }

    ImGui::SameLine();

    if (ImGui::Button("Resume"))
    {
        m_wpModelBlackBoard.lock()->ResumeAnimation();
    }

}