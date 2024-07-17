// default
#include "pch.h"
#include "FrameManager.h"
#include "Frame.h"

HRESULT Engine::CFrameManager::Initialize()
{
	CHECKF(CreateFrame(EFrameLoopType::OUTER), L"Failed to create the outer loop frame.");
	CHECKF(CreateFrame(EFrameLoopType::INNER), L"Failed to create the inner loop frame.");
	return S_OK;
}

float Engine::CFrameManager::Tick(EFrameLoopType _eFrameLoopType)
{
	return m_umapFrames[_eFrameLoopType]->Tick();
}

void Engine::CFrameManager::Release()
{
	for (auto& spTimer : m_umapFrames)
	{
		spTimer.second->Release();
		spTimer.second = nullptr;
	}
	m_umapFrames.clear();
}

shared_ptr<CFrame> Engine::CFrameManager::CreateFrame(EFrameLoopType _eFrameLoopType)
{
	if (m_umapFrames.find(_eFrameLoopType) != m_umapFrames.end()) { return nullptr; }

	const shared_ptr<CFrame> spFrame = CFrame::Create(_eFrameLoopType);
	AddFrame(_eFrameLoopType, spFrame);
	return spFrame;
}

HRESULT Engine::CFrameManager::AddFrame(EFrameLoopType _eFrameLoopType, const shared_ptr<CFrame>& _spFrame)
{
	if (FindFrame(_eFrameLoopType)) { return E_FAIL; }

	m_umapFrames.emplace(_eFrameLoopType, _spFrame);
	return S_OK;
}

HRESULT Engine::CFrameManager::RemoveFrame(EFrameLoopType _eFrameLoopType)
{
	if (shared_ptr<CFrame> spFrame = FindFrame(_eFrameLoopType))
	{
		m_umapFrames.erase(_eFrameLoopType);
		spFrame->Release();
		spFrame = nullptr;
		return S_OK;
	}

	return E_FAIL;
}

shared_ptr<CFrame> Engine::CFrameManager::FindFrame(EFrameLoopType _eFrameLoopType)
{
	if (m_umapFrames.find(_eFrameLoopType) == m_umapFrames.end()) { return nullptr; }

	return m_umapFrames[_eFrameLoopType];
}

optional<float> Engine::CFrameManager::GetDeltaSeconds(EFrameLoopType _eFrameLoopType)
{
	if (shared_ptr<CFrame> spFrame = FindFrame(_eFrameLoopType))
	{
		return spFrame->GetDeltaSeconds();
	}
	return nullopt;
}
