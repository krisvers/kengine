#include <kengine/input/input.hpp>
#include <kengine/input/gamepad.hpp>
#include <kengine/kengine.hpp>
#include <kengine/logger.hpp>
#include <kengine/util/time.hpp>

#include <exception>
#include <functional>

namespace kengine::input {

std::array<bool, static_cast<usize>(InputKey::KEY_COUNT)> input::keys;
std::array<bool, static_cast<usize>(InputKey::KEY_COUNT)> input::prevKeys;
std::vector<Gamepad*> input::gamepads;

void input::init() {
	for (usize i = 0; i < static_cast<usize>(InputKey::KEY_COUNT); ++i) {
		input::keys[i] = false;
		input::prevKeys[i] = false;
	}
}

void input::update() {
	for (usize i = 0; i < static_cast<usize>(InputKey::KEY_COUNT); ++i) {
		input::prevKeys[i] = input::keys[i];
	}
}

void input::destroy() {
	
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

Gamepad* input::getGamepad() {
	return nullptr;
}

} // namespace kengine::input
