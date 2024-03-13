#ifndef KRISVERS_KENGINE_INPUT_INPUT_HPP
#define KRISVERS_KENGINE_INPUT_INPUT_HPP

#include <kengine/window.hpp>
#include <kengine/input/key.hpp>

#include <array>

namespace kengine::input {

class input {
public:
	static bool isKey(InputKey key);
	static bool isKeyDown(InputKey key);
	static bool isKeyUp(InputKey key);

	friend class kengine::Window;

private:
	static std::array<bool, static_cast<usize>(InputKey::KEY_COUNT)> m_keys;
	static std::array<bool, static_cast<usize>(InputKey::KEY_COUNT)> m_prevKeys;

	static void setKey(InputKey key, bool value);
};

} // namespace kengine::input

#endif
