#ifndef KRISVERS_KENGINE_UTIL_VECTOR_HPP
#define KRISVERS_KENGINE_UTIL_VECTOR_HPP

#include <kengine/types.hpp>

#define KENGINE_UTIL_MATRIX_NO_IMPL
#include <kengine/util/matrix.hpp>

#include <array>
#include <initializer_list>
#include <ostream>
#include <string>

namespace kengine::util {

template<usize Width, usize Height>
class Matrix;

template<typename T, usize Count>
class Vector {
public:
	Vector() : m_array() {}
	Vector(T value) { for (usize i = 0; i < Count; ++i) { m_array[i] = value; } }
	Vector(const std::array<T, Count>& values) : m_array(values) {}
	Vector(const std::initializer_list<T>& list) { std::copy(list.begin(), list.end(), m_array.data()); }
	Vector(const Vector& vector) : m_array(vector.m_array) {}

	Vector operator+(const Vector& vector) {
		Vector n = Vector();
		for (usize i = 0; i < Count; ++i) {
			n.m_array = m_array[i] + vector.m_array[i];
		}
		return n;
	}

	Vector operator+=(const Vector& vector) {
		for (usize i = 0; i < Count; ++i) {
			m_array[i] += vector.m_array[i];
		}
		return *this;
	}

	Vector operator-(const Vector& vector) {
		Vector n = Vector();
		for (usize i = 0; i < Count; ++i) {
			n.m_array = m_array[i] - vector.m_array[i];
		}
		return n;
	}

	Vector operator-=(const Vector& vector) {
		for (usize i = 0; i < Count; ++i) {
			m_array[i] -= vector.m_array[i];
		}
		return *this;
	}

	Vector operator*(const Vector& vector) {
		Vector n = Vector();
		for (usize i = 0; i < Count; ++i) {
			n.m_array = m_array[i] * vector.m_array[i];
		}
		return n;
	}

	Vector operator*=(const Vector& vector) {
		for (usize i = 0; i < Count; ++i) {
			m_array[i] *= vector.m_array[i];
		}
		return *this;
	}

	Vector operator/(const Vector& vector) {
		Vector n = Vector();
		for (usize i = 0; i < Count; ++i) {
			n.m_array = m_array[i] / vector.m_array[i];
		}
		return n;
	}

	Vector operator/=(const Vector& vector) {
		for (usize i = 0; i < Count; ++i) {
			m_array[i] /= vector.m_array[i];
		}
		return *this;
	}

	Vector<T, 4> operator*(const Matrix<4, 4>& matrix) {
		static_assert(Count == 3 || Count == 4);
		Vector<f32, Count> n = Vector<f32, Count>();
		for (usize r = 0; r < 4; ++r) {
			for (usize c = 0; c < 4; ++c) {
				n[c] += matrix[r][c] * m_array[r];
			}
		}
		return n;
	}

	Vector mulCross(const Vector& vector) {
		static_assert(Count == 3);

		Vector n = Vector();
		n.m_array[0] = m_array[1] * vector.m_array[2] - m_array[2] * vector.m_array[1];
		n.m_array[1] = m_array[2] * vector.m_array[0] - m_array[0] * vector.m_array[2];
		n.m_array[2] = m_array[0] * vector.m_array[1] - m_array[1] * vector.m_array[0];
		return n;
	}

	f32 mulInner(const Vector& vector) {
		f32 n = 0;
		for (usize i = 0; i < Count; ++i) {
			n += m_array[i] * vector.m_array[i];
		}
		return n;
	}

	f32 magnitude() {
		f32 n = 0;
		for (usize i = 0; i < Count; ++i) {
			n += m_array[i] * m_array[i];
		}
		return sqrt(n);
	}

	Vector& normalize() {
		f32 mag = magnitude();
		if (mag == 0) {
			return *this = 0;
		}

		for (usize i = 0; i < Count; ++i) {
			m_array[i] /= mag;
		}
		return *this;
	}

	Vector normalized() {
		Vector n = Vector();
		f32 mag = magnitude();
		if (mag == 0) {
			return n = 0;
		}

		for (usize i = 0; i < Count; ++i) {
			n.m_array[i] = m_array[i] / mag;
		}
		return n;
	}

	Vector& operator=(const Vector& vector) {
		for (usize i = 0; i < Count; ++i) {
			m_array[i] = vector.m_array[i];
		}
		return *this;
	}

	Vector operator+(T value) {
		Vector n = Vector();
		for (usize i = 0; i < Count; ++i) {
			n.m_array = m_array[i] + value;
		}
		return n;
	}

	Vector& operator+=(T value) {
		for (usize i = 0; i < Count; ++i) {
			m_array[i] += value;
		}
		return *this;
	}

	Vector operator-(T value) {
		Vector n = Vector();
		for (usize i = 0; i < Count; ++i) {
			n.m_array = m_array[i] - value;
		}
		return n;
	}

	Vector& operator-=(T value) {
		for (usize i = 0; i < Count; ++i) {
			m_array[i] -= value;
		}
		return *this;
	}

	Vector operator*(T value) {
		Vector n = Vector();
		for (usize i = 0; i < Count; ++i) {
			n.m_array = m_array[i] * value;
		}
		return n;
	}

	Vector& operator*=(T value) {
		for (usize i = 0; i < Count; ++i) {
			m_array[i] *= value;
		}
		return *this;
	}

	Vector operator/(T value) {
		Vector n = Vector();
		for (usize i = 0; i < Count; ++i) {
			n.m_array = m_array[i] / value;
		}
		return n;
	}

	Vector& operator/=(T value) {
		for (usize i = 0; i < Count; ++i) {
			m_array[i] /= value;
		}
		return *this;
	}

	Vector& operator=(T value) {
		for (usize i = 0; i < Count; ++i) {
			m_array[i] = value;
		}
		return *this;
	}

	bool operator==(const Vector& vector) {
		for (usize i = 0; i < Count; ++i) {
			if (m_array[i] != vector.m_array[i]) {
				return false;
			}
		}
		return true;
	}

	friend std::ostream& operator<<(std::ostream& ostream, const Vector& vector) {
		ostream << "(";
		for (usize i = 0; i < Count; ++i) {
			ostream << vector.m_array[i];
			if (i < Count - 1) {
				ostream << ", ";
			}
		}
		ostream << ")";
		return ostream;
	}

	T& operator[](usize index) { return m_array[index]; }
	const T& operator[](usize index) const { return m_array[index]; }

	const T& x() const { static_assert(Count >= 1); return m_array[0]; }
	const T& y() const { static_assert(Count >= 2); return m_array[1]; }
	const T& z() const { static_assert(Count >= 3); return m_array[2]; }
	const T& w() const { static_assert(Count >= 4); return m_array[3]; }

	std::array<T, Count> array() { std::array<T, Count> arr = m_array; return arr; }
	constexpr auto begin() noexcept { return m_array.begin(); }
	constexpr auto end() noexcept { return m_array.end(); }
	constexpr T* data() noexcept { return m_array.data(); }

	static constexpr kengine::usize size() { return sizeof(T) * Count; }
	static constexpr kengine::usize count() { return Count; }

private:
	std::array<T, Count> m_array;
};

} // namespace kengine::util

#endif
