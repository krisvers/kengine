#ifndef KRISVERS_KENGINE_INPUT_INPUT_HPP
#define KRISVERS_KENGINE_INPUT_INPUT_HPP

#include <kengine/window.hpp>
#include <kengine/input/key.hpp>

#include <array>
#include <vector>

namespace kengine::input {

class Gamepad;

class input {
public:
	static void init();
	static void update();
	static void destroy();

	static bool isKey(InputKey key);
	static bool isKeyDown(InputKey key);
	static bool isKeyUp(InputKey key);

	static Gamepad* getGamepad();
	static Gamepad* getGamepad(usize controller);

	friend class kengine::Window;

private:
	static std::array<bool, static_cast<usize>(InputKey::KEY_COUNT)> keys;
	static std::array<bool, static_cast<usize>(InputKey::KEY_COUNT)> prevKeys;
	static std::vector<Gamepad*> gamepads;

	static void setKey(InputKey key, bool value);
};

} // namespace kengine::input

#endif
