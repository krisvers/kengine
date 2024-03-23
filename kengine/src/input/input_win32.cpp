#include <kengine/input/input.hpp>
#include <kengine/input/gamepad.hpp>
#include <kengine/kengine.hpp>
#include <kengine/logger.hpp>
#include <kengine/util/time.hpp>

#include <Windows.h>

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <Dbt.h>
#include <Hidclass.h>

#include <atlstr.h>

#include <exception>
#include <functional>

namespace kengine::input {

std::array<bool, static_cast<usize>(InputKey::KEY_COUNT)> input::keys;
std::array<bool, static_cast<usize>(InputKey::KEY_COUNT)> input::prevKeys;
std::vector<Gamepad*> input::gamepads;

static IDirectInput8* g_directInput;

class GamepadDirectInput : public Gamepad {
public:
	virtual bool isButton(GamepadButton button);
	virtual bool isButtonDown(GamepadButton button);
	virtual bool isButtonUp(GamepadButton button);

	virtual f32 getAxis(GamepadAxis axis);
	virtual math::Vector<f32, 2> getStick(GamepadStick stick);

	virtual math::Vector<f32, 2> getStickDeadzone(GamepadStick stick);
	virtual void setStickDeadzone(GamepadStick stick, math::Vector<f32, 2> deadzone);

	void disconnect() {
		logger::printf(LogType::VERBOSE, "Controller \"%s\" disconnected %zu\n", deviceName, m_controller);
		connected = false;
		zero();
		m_device->Unacquire();
		m_device->Release();
		m_device = nullptr;
		m_deviceGUID = GUID();
	}

	GamepadDirectInput() {
		m_stickDeadzones[0] = { 0.10f, 0.10f };
		m_stickDeadzones[1] = { 0.10f, 0.10f };
	}

private:
	u32 m_controller;
	u32 m_buttons[16];
	u32 m_prevButtons[16];
	f32 m_axes[6];
	math::Vector<f32, 2> m_stickDeadzones[2];

	IDirectInputDevice8* m_device;
	GUID m_deviceGUID;

	void zero() {
		for (usize i = 0; i < 16; ++i) {
			if (i < 6) {
				m_axes[i] = 0.0f;
			}

			m_buttons[i] = false;
			m_prevButtons[i] = false;
		}
	}

	friend class input;
};

void input::init() {
	for (usize i = 0; i < static_cast<usize>(InputKey::KEY_COUNT); ++i) {
		input::keys[i] = false;
		input::prevKeys[i] = false;
	}

	HRESULT result = DirectInput8Create(GetModuleHandle(nullptr), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&g_directInput, nullptr);
	if (FAILED(result)) {
		throw std::runtime_error("Failed to create DirectInput8");
	}

	g_directInput->EnumDevices(DI8DEVCLASS_GAMECTRL, [](LPCDIDEVICEINSTANCE dev, LPVOID ref) -> BOOL {
		IDirectInputDevice8* device;
		HRESULT result = g_directInput->CreateDevice(dev->guidInstance, &device, nullptr);
		if (FAILED(result)) {
			throw std::runtime_error("Failed to create DirectInput device");
		}

		result = device->SetDataFormat(&c_dfDIJoystick2);
		if (FAILED(result)) {
			throw std::runtime_error("Failed to set DirectInput device data format");
		}

		result = device->SetCooperativeLevel(static_cast<HWND>(kengine::KEngine::getWindow()->getHandleWin32()), DISCL_EXCLUSIVE | DISCL_FOREGROUND);
		if (FAILED(result)) {
			throw std::runtime_error("Failed to set DirectInput device cooperative level");
		}

		DIPROPDWORD dipdw;
		dipdw.diph.dwSize = sizeof(DIPROPDWORD);
		dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
		dipdw.diph.dwObj = 0;
		dipdw.diph.dwHow = DIPH_DEVICE;
		dipdw.dwData = 1000;
		
		result = device->SetProperty(DIPROP_BUFFERSIZE, &dipdw.diph);
		if (FAILED(result)) {
			throw std::runtime_error("Failed to set DirectInput device buffer size");
		}

		result = device->Acquire();
		if (FAILED(result)) {
			device->Release();
			return TRUE;
		}

		GamepadDirectInput* gamepad = new GamepadDirectInput();
		gamepad->m_device = device;
		gamepad->m_controller = static_cast<u32>(input::gamepads.size());
		gamepad->m_deviceGUID = dev->guidInstance;
		gamepad->connected = true;

		CW2A cstring(dev->tszInstanceName, CP_UTF8);
		memcpy(const_cast<void*>(reinterpret_cast<const void*>(gamepad->deviceName)), cstring.m_psz, 32);

		input::gamepads.push_back(gamepad);
		logger::printf(LogType::VERBOSE, "Found gamepad: %S\n", dev->tszProductName);
		return TRUE;
	}, nullptr, DIEDFL_ATTACHEDONLY);
}

void input::update() {
	for (usize i = 0; i < static_cast<usize>(InputKey::KEY_COUNT); ++i) {
		input::prevKeys[i] = input::keys[i];
	}

	for (usize i = 0; i < input::gamepads.size(); ++i) {
		GamepadDirectInput* gamepad = static_cast<GamepadDirectInput*>(input::gamepads[i]);
		if (!gamepad->connected) {
			continue;
		}

		DIJOYSTATE2 state;
		HRESULT result = gamepad->m_device->Poll();
		if (FAILED(result)) {
			result = gamepad->m_device->Acquire();
			if (FAILED(result)) {
				gamepad->disconnect();
				continue;
			}
		}

		result = gamepad->m_device->GetDeviceState(sizeof(DIJOYSTATE2), &state);
		if (FAILED(result)) {
			gamepad->disconnect();
			continue;
		}

		for (usize j = 0; j < 16; ++j) {
			gamepad->m_prevButtons[j] = gamepad->m_buttons[j];
		}

		gamepad->m_buttons[2] = state.rgbButtons[0] & 0x80;
		gamepad->m_buttons[0] = state.rgbButtons[1] & 0x80;
		gamepad->m_buttons[1] = state.rgbButtons[2] & 0x80;
		gamepad->m_buttons[3] = state.rgbButtons[3] & 0x80;
		gamepad->m_buttons[9] = state.rgbButtons[4] & 0x80;
		gamepad->m_buttons[10] = state.rgbButtons[5] & 0x80;
		gamepad->m_buttons[4] = state.rgbButtons[8] & 0x80;
		gamepad->m_buttons[6] = state.rgbButtons[9] & 0x80;
		gamepad->m_buttons[7] = state.rgbButtons[10] & 0x80;
		gamepad->m_buttons[8] = state.rgbButtons[11] & 0x80;
		gamepad->m_buttons[5] = state.rgbButtons[13] & 0x80;

		if (state.rgdwPOV[0] > 36000) {
			gamepad->m_buttons[11] = false;	
		} else {
			gamepad->m_buttons[11] = (state.rgdwPOV[0] >= 31500 || state.rgdwPOV[0] <= 4500);
		}
		gamepad->m_buttons[12] = (state.rgdwPOV[0] >= 13500 && state.rgdwPOV[0] <= 22500);
		gamepad->m_buttons[13] = (state.rgdwPOV[0] >= 22500 && state.rgdwPOV[0] <= 31500);
		gamepad->m_buttons[14] = (state.rgdwPOV[0] >= 4500 && state.rgdwPOV[0] <= 13500);

		gamepad->m_axes[0] = state.lX / 65535.0f * 2.0f - 1.0f;
		gamepad->m_axes[1] = -(state.lY / 65535.0f * 2.0f - 1.0f);
		gamepad->m_axes[2] = state.lZ / 65535.0f * 2.0f - 1.0f;
		gamepad->m_axes[3] = -(state.lRz / 65535.0f * 2.0f - 1.0f);
		gamepad->m_axes[4] = state.lRx / 65535.0f;
		gamepad->m_axes[5] = state.lRy / 65535.0f;

		if (gamepad->m_axes[0] <= gamepad->m_stickDeadzones[0][0] && gamepad->m_axes[0] >= -gamepad->m_stickDeadzones[0][0]) {
			gamepad->m_axes[0] = 0.0f;
		}
		if (gamepad->m_axes[1] <= gamepad->m_stickDeadzones[0][1] && gamepad->m_axes[1] >= -gamepad->m_stickDeadzones[0][1]) {
			gamepad->m_axes[1] = 0.0f;
		}
		if (gamepad->m_axes[2] <= gamepad->m_stickDeadzones[1][0] && gamepad->m_axes[2] >= -gamepad->m_stickDeadzones[1][0]) {
			gamepad->m_axes[2] = 0.0f;
		}
		if (gamepad->m_axes[3] <= gamepad->m_stickDeadzones[1][1] && gamepad->m_axes[3] >= -gamepad->m_stickDeadzones[1][1]) {
			gamepad->m_axes[3] = 0.0f;
		}
	}
}

void input::destroy() {
	for (usize i = 0; i < input::gamepads.size(); ++i) {
		GamepadDirectInput* gamepad = static_cast<GamepadDirectInput*>(input::gamepads[i]);
		if (!gamepad->connected) {
			continue;
		}

		gamepad->m_device->Unacquire();
		gamepad->m_device->Release();
		delete input::gamepads[i];
	}

	g_directInput->Release();
}

bool input::isKey(InputKey key) {
	return input::keys[static_cast<u32>(key)];
}

bool input::isKeyDown(InputKey key) {
	return input::keys[static_cast<u32>(key)] && !input::prevKeys[static_cast<u32>(key)];
}

bool input::isKeyUp(InputKey key) {
	return !input::keys[static_cast<u32>(key)] && input::prevKeys[static_cast<u32>(key)];
}

void input::setKey(InputKey key, bool value) {
	if (key >= InputKey::KEY_COUNT || key < static_cast<InputKey>(0)) {
		return;
	}

	input::keys[static_cast<u32>(key)] = value;
}

bool GamepadDirectInput::isButton(GamepadButton button) {
	return m_buttons[static_cast<u32>(button)];
}

bool GamepadDirectInput::isButtonDown(GamepadButton button) {
	return m_buttons[static_cast<u32>(button)] && !m_prevButtons[static_cast<u32>(button)];
}

bool GamepadDirectInput::isButtonUp(GamepadButton button) {
	return !m_buttons[static_cast<u32>(button)] && m_prevButtons[static_cast<u32>(button)];
}

f32 GamepadDirectInput::getAxis(GamepadAxis axis) {
	return m_axes[static_cast<u32>(axis)];
}

math::Vector<f32, 2> GamepadDirectInput::getStick(GamepadStick stick) {
	math::Vector<f32, 2> vector;
	if (stick == GamepadStick::STICK_LEFT) {
		vector[0] = m_axes[0];
		vector[1] = m_axes[1];
	} else if (stick == GamepadStick::STICK_RIGHT) {
		vector[0] = m_axes[2];
		vector[1] = m_axes[3];
	}

	return vector;
}

math::Vector<f32, 2> GamepadDirectInput::getStickDeadzone(GamepadStick stick) {
	return m_stickDeadzones[static_cast<u32>(stick)];
}

void GamepadDirectInput::setStickDeadzone(GamepadStick stick, math::Vector<f32, 2> deadzone) {
	m_stickDeadzones[static_cast<u32>(stick)] = deadzone;
}

Gamepad* input::getGamepad() {
	for (Gamepad*& gamepad : gamepads) {
		if (gamepad->connected) {
			return gamepad;
		}
	}

	return nullptr;
}

} // namespace kengine::input
