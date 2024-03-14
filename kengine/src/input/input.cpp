#include <kengine/input/input.hpp>

namespace kengine::input {

std::array<bool, static_cast<usize>(InputKey::KEY_COUNT)> input::m_keys;
std::array<bool, static_cast<usize>(InputKey::KEY_COUNT)> input::m_prevKeys;

bool input::isKey(InputKey key) {
	return input::m_keys[static_cast<u32>(key)];
}

bool input::isKeyDown(InputKey key) {
	return input::m_keys[static_cast<u32>(key)] && !input::m_prevKeys[static_cast<u32>(key)];
}

bool input::isKeyUp(InputKey key) {
	return !input::m_keys[static_cast<u32>(key)] && input::m_prevKeys[static_cast<u32>(key)];
}

void input::setKey(InputKey key, bool value) {
	input::m_keys[static_cast<u32>(key)] = value;
}

} // namespace kengine::input
