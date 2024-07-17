#include "pch.h"
#include "InputManager.h"

#include "EngineModule.h"
#include "DeviceManager.h"

HRESULT Engine::CInputManager::Initialize()
{
	m_wpDeviceManager = CEngineModule::GetInstance()->GetDeviceManager();
	m_hWnd = m_wpDeviceManager.lock()->GetWindowHandle();

	m_fWindowSizeX = static_cast<float>(m_wpDeviceManager.lock()->GetWindowSizeX());
	m_fWindowSizeY = static_cast<float>(m_wpDeviceManager.lock()->GetWindowSizeY());

	// 컴 객체를 생성합니다.
	ENSURE(::DirectInput8Create(m_wpDeviceManager.lock()->GetInstanceHandle(), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_pDeviceInput, NULL));

	// 키보드 객체를 생성합니다.
	ENSURE(m_pDeviceInput->CreateDevice(GUID_SysKeyboard, &m_pDIKeyDevice, nullptr));
	ENSURE(m_pDIKeyDevice->SetDataFormat(&c_dfDIKeyboard));
	ENSURE(m_pDIKeyDevice->SetCooperativeLevel(m_wpDeviceManager.lock()->GetWindowHandle(), DISCL_BACKGROUND | DISCL_NONEXCLUSIVE));

	while (m_pDIKeyDevice->Acquire() == DIERR_INPUTLOST);

	// 마우스 객체를 생성합니다.
	ENSURE(m_pDeviceInput->CreateDevice(GUID_SysMouse, &m_pDIMouseDevice, nullptr));
	ENSURE(m_pDIMouseDevice->SetDataFormat(&c_dfDIMouse));
	ENSURE(m_pDIMouseDevice->SetCooperativeLevel(m_wpDeviceManager.lock()->GetWindowHandle(), DISCL_BACKGROUND | DISCL_NONEXCLUSIVE));

	while (m_pDIMouseDevice->Acquire() == DIERR_INPUTLOST);

	return S_OK;
}

int32 Engine::CInputManager::Tick(float _fDeltaSeconds)
{
	// 키보드 입력을 업데이트 합니다.
	int8 iAsciiKeys[256];
	ENSURE(m_pDIKeyDevice->GetDeviceState(256, iAsciiKeys));

	for (uint32 iKey = 0; iKey < 256; iKey++)
	{
		// 키를 누른 경우
		if (iAsciiKeys[iKey] & 0x80)
		{
			switch (m_arrKeyStates[iKey])
			{
			// 이전에 눌렀거나 누르는 중인 경우
			case EKeyState::DOWN:
			case EKeyState::PRESSED:
				m_arrKeyStates[iKey] = EKeyState::PRESSED;
				break;

			// 이전에 뗐거나 떼는 중인 경우
			case EKeyState::UP:
			case EKeyState::RELEASED:
				m_arrKeyStates[iKey] = EKeyState::DOWN;
				break;
			}
		}

		// 키를 누르지 않은 경우
		else
		{
			switch (m_arrKeyStates[iKey])
			{
			// 이전에 눌렀거나 누르는 중인 경우
			case EKeyState::DOWN:
			case EKeyState::PRESSED:
				m_arrKeyStates[iKey] = EKeyState::UP;
				break;

			// 이전에 뗐거나 떼는 중인 경우
			case EKeyState::UP:
			case EKeyState::RELEASED:
				m_arrKeyStates[iKey] = EKeyState::RELEASED;
				break;
			}
		}
	}

	// 마우스 입력을 업데이트 합니다.
	DIMOUSESTATE tMouseState;
	ENSURE(m_pDIMouseDevice->GetDeviceState(sizeof(tMouseState), &tMouseState));

	for (uint32 iMouse = 0; iMouse <= 3; iMouse++)
	{
		// 키를 누른 경우
		if (tMouseState.rgbButtons[iMouse])
		{
			switch (m_arrMouseStates[iMouse])
			{
			// 이전에 눌렀거나 누르는 중인 경우
			case EMouseState::DOWN:
			case EMouseState::PRESSED:
				m_arrMouseStates[iMouse] = EMouseState::PRESSED;
				break;

			// 이전에 뗐거나 떼는 중인 경우
			case EMouseState::UP:
			case EMouseState::RELEASED:
				m_arrMouseStates[iMouse] = EMouseState::DOWN;
				break;
			}
		}

		// 키를 누르지 않은 경우
		else
		{
			switch (m_arrMouseStates[iMouse])
			{
			// 이전에 눌렀거나 누르는 중인 경우
			case EMouseState::DOWN:
			case EMouseState::PRESSED:
				m_arrMouseStates[iMouse] = EMouseState::UP;
				break;

			// 이전에 뗐거나 떼는 중인 경우
			case EMouseState::UP:
			case EMouseState::RELEASED:
				m_arrMouseStates[iMouse] = EMouseState::RELEASED;
				break;
			}
		}
	}

	// 마우스의 움직임을 업데이트합니다.
	m_arrMouseDelta[0] = tMouseState.lX;
	m_arrMouseDelta[1] = tMouseState.lY;
	m_arrMouseDelta[2] = tMouseState.lZ;
	
	if (m_bCursorLocked)
	{
		::SetCursorPos(m_tCursorPoint.x, m_tCursorPoint.y);
	}
	return 0;
}

void Engine::CInputManager::Release()
{
	ENSURE(m_pDIKeyDevice->Unacquire());
	if (m_pDIKeyDevice->Release()) { MESSAGE_BOX(TEXT("Failed to release LPDIRECTINPUTDEVICE8 at CInputManager.")); }
	m_pDIKeyDevice = nullptr;

	ENSURE(m_pDIMouseDevice->Unacquire());
	if (m_pDIMouseDevice->Release()) { MESSAGE_BOX(TEXT("Failed to release LPDIRECTINPUTDEVICE8 at CInputManager.")); }
	m_pDIMouseDevice = nullptr;

	if (m_pDeviceInput->Release()) { MESSAGE_BOX(TEXT("Failed to release LPDIRECTINPUT8 at CInputManager.")); }
	m_pDeviceInput = nullptr;
}

std::pair<int32, int32> Engine::CInputManager::GetCursorScreenPosition()
{
	::GetCursorPos(&m_tCursorPoint);
	::ScreenToClient(m_hWnd, &m_tCursorPoint);
	return { m_tCursorPoint.x, m_tCursorPoint.y };
}

std::pair<float, float> Engine::CInputManager::GetCursorNDCPosition()
{
	std::pair<int32, int32> prCursorPosition = GetCursorScreenPosition();
	std::pair<float, float> prCursorNDCPosition{ 0.0f, 0.0f };

	prCursorNDCPosition.first = (prCursorPosition.first * 2.0f / m_fWindowSizeX) - 1.0f;
	prCursorNDCPosition.second = (-prCursorPosition.second * 2.0f / m_fWindowSizeY) + 1.0f;

	prCursorNDCPosition.first = std::clamp(prCursorNDCPosition.first, -1.0f, 1.0f);
	prCursorNDCPosition.second = std::clamp(prCursorNDCPosition.second, -1.0f, 1.0f);

	return prCursorNDCPosition;
}
