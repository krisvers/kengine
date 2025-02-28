#ifndef KENGINE_UTIL_MATH_VECTOR_HPP
#define KENGINE_UTIL_MATH_VECTOR_HPP

#include <kengine/types.hpp>
#include <kengine/util/math/sqrt.hpp>

#ifdef min
#undef min
#endif

#include <algorithm>

namespace kengine::util::math {

template<typename T, kengine::usize N>
class Vector {
public:
	Vector() {
		for (kengine::usize i = 0; i < N; ++i) {
			_data[i] = T();
		}
	}

	Vector(const T& value) {
		for (kengine::usize i = 0; i < N; ++i) {
			_data[i] = value;
		}
	}

	Vector(const T* data) {
		for (kengine::usize i = 0; i < N; ++i) {
			_data[i] = data[i];
		}
	}

	Vector(const Vector<T, N>& other) {
		for (kengine::usize i = 0; i < N; ++i) {
			_data[i] = other._data[i];
		}
	}

	Vector(Vector<T, N>&& other) {
		for (kengine::usize i = 0; i < N; ++i) {
			_data[i] = other._data[i];
		}
	}

	Vector(const std::initializer_list<T>& list) {
		std::copy(list.begin(), list.end(), _data);
	}

	Vector<T, N>& operator=(const Vector<T, N>& other) {
		for (kengine::usize i = 0; i < N; ++i) {
			_data[i] = other._data[i];
		}
		return *this;
	}

	Vector<T, N>& operator=(Vector<T, N>&& other) {
		for (kengine::usize i = 0; i < N; ++i) {
			_data[i] = other._data[i];
		}
		return *this;
	}

	T& operator[](kengine::usize index) {
		return _data[index];
	}

	const T& operator[](kengine::usize index) const {
		return _data[index];
	}

	Vector<T, N> operator+(const Vector<T, N>& other) const {
		Vector<T, N> result;
		for (kengine::usize i = 0; i < N; ++i) {
			result._data[i] = _data[i] + other._data[i];
		}
		return result;
	}

	Vector<T, N> operator-(const Vector<T, N>& other) const {
		Vector<T, N> result;
		for (kengine::usize i = 0; i < N; ++i) {
			result._data[i] = _data[i] - other._data[i];
		}
		return result;
	}

	Vector<T, N> operator*(const T& scalar) const {
		Vector<T, N> result;
		for (kengine::usize i = 0; i < N; ++i) {
			result._data[i] = _data[i] * scalar;
		}
		return result;
	}

	Vector<T, N> operator/(const T& scalar) const {
		Vector<T, N> result;
		for (kengine::usize i = 0; i < N; ++i) {
			result._data[i] = _data[i] / scalar;
		}
		return result;
	}

	Vector<T, N>& operator+=(const Vector<T, N>& other) {
		for (kengine::usize i = 0; i < N; ++i) {
			_data[i] += other._data[i];
		}
		return *this;
	}

	Vector<T, N>& operator-=(const Vector<T, N>& other) {
		for (kengine::usize i = 0; i < N; ++i) {
			_data[i] -= other._data[i];
		}
		return *this;
	}

	Vector<T, N>& operator*=(const T& scalar) {
		for (kengine::usize i = 0; i < N; ++i) {
			_data[i] *= scalar;
		}
		return *this;
	}

	Vector<T, N>& operator/=(const T& scalar) {
		for (kengine::usize i = 0; i < N; ++i) {
			_data[i] /= scalar;
		}
		return *this;
	}

	bool operator==(const Vector<T, N>& other) const {
		for (kengine::usize i = 0; i < N; ++i) {
			if (_data[i] != other._data[i]) {
				return false;
			}
		}
		return true;
	}

	bool operator!=(const Vector<T, N>& other) const {
		return !(*this == other);
	}

	T dot(const Vector<T, N>& other) const {
		T result = T();
		for (kengine::usize i = 0; i < N; ++i) {
			result += _data[i] * other._data[i];
		}
		return result;
	}

	T magnitude() const {
		return kengine::util::math::sqrt(dot(*this));
	}

	Vector<T, N> normalize() const {
		return *this / magnitude();
	}

	T* data() {
		return _data;
	}

	const T* data() const {
		return _data;
	}

private:
	T _data[N];
};

} // namespace kengine::util::math

#endif