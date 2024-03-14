#ifndef KRISVERS_KENGINE_UTIL_VECTOR_HPP
#define KRISVERS_KENGINE_UTIL_VECTOR_HPP

#include <kengine/types.hpp>

#include <array>

namespace kengine::util {

template<typename T, usize Count>
class Vector {
public:
	Vector(std::array<T, Count> values) : m_array(values) {}
	Vector(std::initializer_list<T> list) : m_array(list) {}

	Vector operator+(Vector const& vector);
	Vector operator-(Vector const& vector);
	Vector operator*(Vector const& vector);
	Vector operator/(Vector const& vector);
	Vector& operator=(Vector const& vector);
	
	bool operator==(Vector const& vector);

	std::array<T, Count> array() { std::array<T, Count> arr = m_array; return arr; }
	static constexpr kengine::usize size() { return sizeof(T) * Count; }
	static constexpr kengine::usize count() { return Count; }

private:
	std::array<T, Count> m_array;
};

} // namespace kengine::util

#endif
