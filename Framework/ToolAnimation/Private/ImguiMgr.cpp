#include "pch.h"
#include "ImguiMgr.h"

#include "EngineModule.h"
#include "DeviceManager.h"
#include "WindowBase.h"
#include "Level.h"

#include "Sequencer.h"
#include "NotifyTab.h"
#include "AnimInspector.h"

IMPLEMENT_SINGLETON(CImguiMgr)

HRESULT ToolAnimation::CImguiMgr::Initialize()
{
	// Imgui를 띄우기 위해, 장치를 받아옵니다.
	m_pDevice = CEngineModule::GetInstance()->GetDeviceManager()->GetDevice();
	m_pDeviceContext = CEngineModule::GetInstance()->GetDeviceManager()->GetDeviceContext();

	// Setup ImGui Context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); 
	(void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.Fonts->AddFontFromFileTTF("../../Resource/Fonts/NotoSansKR-Regular.ttf", 16.0f, nullptr, io.Fonts->GetGlyphRangesKorean());
	
	// Setup Dear ImGui style
	//ImGui::StyleColorsDark();

	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(g_hWnd);
	ImGui_ImplDX11_Init(m_pDevice.Get(), m_pDeviceContext.Get());

	InitWindow();

	return S_OK;
}

void ToolAnimation::CImguiMgr::Tick(float _fDeltaSeconds)
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();

	ImGui::NewFrame();
	// ImGuizmo::BeginFrame();

	for (auto& window : m_lstWindow)
	{
		window->Tick(_fDeltaSeconds);
	}
}

void ToolAnimation::CImguiMgr::Render()
{
	ImGui::Render();

	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags /*& ImGuiConfigFlags_ViewportsEnable*/)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}
}

void ToolAnimation::CImguiMgr::Release()
{
	if (m_pDevice) { m_pDevice = nullptr; }
	if (m_pDeviceContext) { m_pDeviceContext = nullptr; }

	for (auto& window : m_lstWindow)
	{
		window->Release();
		window = nullptr;
	}

	m_lstWindow.clear();

	ImGui_ImplWin32_Shutdown();
	ImGui_ImplDX11_Shutdown();
	ImGui::DestroyContext();

	m_spInstance = nullptr;
}

void ToolAnimation::CImguiMgr::InitWindow()
{
	{
		shared_ptr<CSequencer> spSequencer = make_shared<CSequencer>();
		spSequencer->SetCurrentLevel(m_spCurrentLevel);
		spSequencer->Initialize();

		m_lstWindow.push_back(spSequencer);
		m_wpSeqencer = spSequencer;
	}
	
	{
		shared_ptr<CNotifyTab> spNotifyTab = make_shared<CNotifyTab>();
		spNotifyTab->SetCurrentLevel(m_spCurrentLevel);
		spNotifyTab->Initialize();

		m_lstWindow.push_back(spNotifyTab);
		m_wpNotifyTab = spNotifyTab;
	}

	{
		shared_ptr<CAnimInspector> spInspector = make_shared<CAnimInspector>();
		spInspector->SetCurrentLevel(m_spCurrentLevel);
		spInspector->Initialize();

		m_lstWindow.push_back(spInspector);
		m_wpInspector = spInspector;
	}
}

void ToolAnimation::CImguiMgr::SetCurrentLevel(shared_ptr<CLevel> _spCurLevel)
{
	m_spCurrentLevel = _spCurLevel;
}
