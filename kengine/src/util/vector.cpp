#include <kengine/util/vector.hpp>

namespace kengine::util {

template<typename T, usize Count>
Vector<T, Count> Vector<T, Count>::operator+(Vector<T, Count> const& vector) {
	Vector n = Vector();
	for (usize i = 0; i < Count; ++i) {
		n.m_array = m_array[i] + vector.m_array[i];
	}
	return n;
}

template<typename T, usize Count>
Vector<T, Count> Vector<T, Count>::operator-(Vector<T, Count> const& vector) {
	Vector n = Vector();
	for (usize i = 0; i < Count; ++i) {
		n.m_array = m_array[i] - vector.m_array[i];
	}
	return n;
}

template<typename T, usize Count>
Vector<T, Count> Vector<T, Count>::operator*(Vector<T, Count> const& vector) {
	Vector n = Vector();
	for (usize i = 0; i < Count; ++i) {
		n.m_array = m_array[i] * vector.m_array[i];
	}
	return n;
}

template<typename T, usize Count>
Vector<T, Count> Vector<T, Count>::operator/(Vector<T, Count> const& vector) {
	Vector n = Vector();
	for (usize i = 0; i < Count; ++i) {
		n.m_array = m_array[i] / vector.m_array[i];
	}
	return n;
}

template<typename T, usize Count>
Vector<T, Count>& Vector<T, Count>::operator=(Vector<T, Count> const& vector) {
	for (usize i = 0; i < Count; ++i) {
		m_array[i] = vector.m_array[i];
	}
	return *this;
}

template<typename T, usize Count>
bool Vector<T, Count>::operator==(Vector<T, Count> const& vector) {
	for (usize i = 0; i < Count; ++i) {
		if (m_array[i] != vector.m_array[i]) {
			return false;
		}
	}
	return true;
}

} // namespace kengine::util
