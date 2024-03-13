#include <kengine/input/input.hpp>

namespace kengine::input {

bool input::isKey(InputKey key) {
	return m_keys[static_cast<u32>(key)];
}

bool input::isKeyDown(InputKey key) {
	return m_keys[static_cast<u32>(key)] && !m_prevKeys[static_cast<u32>(key)];
}

bool input::isKeyUp(InputKey key) {
	return !m_keys[static_cast<u32>(key)] && m_prevKeys[static_cast<u32>(key)];
}

void input::setKey(InputKey key, bool value) {
	m_keys[static_cast<u32>(key)] = value;
}

} // namespace kengine::input
