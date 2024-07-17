#pragma once
#include "Manager.h"

namespace Engine
{
	class CLevel;
}

namespace ToolAnimation
{
	class CWindowBase;
	class CSequencer;
	class CNotifyTab;
	class CAnimInspector;

	class CImguiMgr : public CManager
	{
		DECLARE_SINGLETON(CImguiMgr)

	public:
		HRESULT Initialize();

		void Tick(float _fDeltaSeconds);
		void Render();
		void Release();

	private:
		void InitWindow();

	public:
		void SetCurrentLevel(shared_ptr<CLevel> _spCurLevel);

	private:
		ComPtr<ID3D11Device> m_pDevice = nullptr;
		ComPtr<ID3D11DeviceContext> m_pDeviceContext = nullptr;

		shared_ptr<CLevel> m_spCurrentLevel = nullptr;

		// �ϰ� ó���� ��ü ������
		list<shared_ptr<CWindowBase>> m_lstWindow;

		// ���� ������ weak ptr ������
		weak_ptr<CSequencer> m_wpSeqencer;
		weak_ptr<CNotifyTab> m_wpNotifyTab;
		weak_ptr<CAnimInspector> m_wpInspector;
	};
}