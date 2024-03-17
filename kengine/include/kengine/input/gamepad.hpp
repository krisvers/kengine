#ifndef KRISVERS_KENGINE_INPUT_GAMEPAD_HPP
#define KRISVERS_KENGINE_INPUT_GAMEPAD_HPP

#include <kengine/types.hpp>
#include <kengine/util/vector.hpp>

namespace kengine::input {

enum class GamepadButton {
	BUTTON_A,
	BUTTON_B,
	BUTTON_X,
	BUTTON_Y,
	BUTTON_BACK,
	BUTTON_GUIDE,
	BUTTON_START,
	BUTTON_LEFT_STICK,
	BUTTON_RIGHT_STICK,
	BUTTON_LEFT_SHOULDER,
	BUTTON_RIGHT_SHOULDER,
	BUTTON_DPAD_UP,
	BUTTON_DPAD_DOWN,
	BUTTON_DPAD_LEFT,
	BUTTON_DPAD_RIGHT,
	BUTTON_COUNT
};

enum class GamepadAxis {
	AXIS_LEFT_X,
	AXIS_LEFT_Y,
	AXIS_RIGHT_X,
	AXIS_RIGHT_Y,
	AXIS_TRIGGER_LEFT,
	AXIS_TRIGGER_RIGHT,
	AXIS_COUNT
};

enum class GamepadStick {
	STICK_LEFT,
	STICK_RIGHT,
	STICK_COUNT
};

class Gamepad {
public:
	bool connected = false;

	virtual bool isButton(GamepadButton button) = 0;
	virtual bool isButtonDown(GamepadButton button) = 0;
	virtual bool isButtonUp(GamepadButton button) = 0;

	virtual f32 getAxis(GamepadAxis axis) = 0;
	virtual util::Vector<f32, 2> getStick(GamepadStick stick) = 0;

	virtual util::Vector<f32, 2> getStickDeadzone(GamepadStick stick) = 0;
	virtual void setStickDeadzone(GamepadStick stick, util::Vector<f32, 2> deadzone) = 0;

	friend class input;
};

} // namespace kengine::input

#endif