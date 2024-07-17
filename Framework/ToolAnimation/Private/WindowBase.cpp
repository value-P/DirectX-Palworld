#include "pch.h"
#include "WindowBase.h"
#include "ModelBlackBoard.h"

void ToolAnimation::CWindowBase::Initialize()
{
	m_wpModelBlackBoard = CModelBlackBoard::GetInstance();

	m_eWindowFlag |= ImGuiWindowFlags_NoCollapse;
	m_eWindowFlag |= ImGuiWindowFlags_NoNav;
	m_eWindowFlag |= ImGuiWindowFlags_NoMove;
}

void ToolAnimation::CWindowBase::BeginPlay()
{
}

void ToolAnimation::CWindowBase::PreTick(float _fDeltaSeconds)
{
}

void ToolAnimation::CWindowBase::Tick(float _fDeltaSeconds)
{
	optional<FAnimationData> optAnimData = m_wpModelBlackBoard.lock()->GetCurrentModelAnimData();
	if (optAnimData != nullopt)
	{
		m_tAnimData = optAnimData.value();
	}
}

void ToolAnimation::CWindowBase::PostTick(float _fDeltaSeconds)
{
}

void ToolAnimation::CWindowBase::EndPlay()
{
}

void ToolAnimation::CWindowBase::Release()
{
}

void ToolAnimation::CWindowBase::SetCurrentLevel(shared_ptr<CLevel> _spCurLevel)
{
	m_wpLevel = _spCurLevel;
}
