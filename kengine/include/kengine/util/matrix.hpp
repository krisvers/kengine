#ifndef KRISVERS_KENGINE_UTIL_MATRIX_HPP
#define KRISVERS_KENGINE_UTIL_MATRIX_HPP

#include <kengine/types.hpp>

#include <cmath>
#include <array>
#include <stdexcept>

namespace kengine::util {

#ifndef KRISVERS_KENGINE_UTIL_VECTOR_HPP
template<typename T, usize Count>
class Vector;
#endif

template<usize Width, usize Height>
class Matrix {
public:
	Matrix(const std::array<f32, Width * Height>& values) : m_array(values) {}
	Matrix(const std::initializer_list<f32>& list) { std::copy(list.begin(), list.end(), m_array.data()); }
	Matrix(const Matrix& matrix) : m_array(matrix.m_array) {}
	Matrix() : m_array() {}

	Matrix& zero() {
		for (usize i = 0; i < Width * Height; i++) {
			m_array[i] = 0;
		}
		return *this;
	}

	Matrix& identity() {
		zero();
		for (usize i = 0; i < Width; i++) {
			m_array[i + i * Width] = 1;
		}
		return *this;
	}

	Matrix& translate(f32 x, f32 y, f32 z) {
		m_array[Width * 3] = x;
		m_array[Width * 3 + 1] = y;
		m_array[Width * 3 + 2] = z;
		return *this;
	}

	Matrix& translate(const Vector<f32, 3>& vector) {
		m_array[Width * 3] = vector[0];
		m_array[Width * 3 + 1] = vector[1];
		m_array[Width * 3 + 2] = vector[2];
		return *this;
	}

	Matrix& translateInPlace(f32 x, f32 y, f32 z) {
		Matrix n = Matrix();
		for (usize i = 0; i < Width; ++i) {
			Vector<f32, 4> row4 = operator[](i);
			Vector<f32, 3> row{ row4[0], row4[1], row4[2] };
			m_array[Width * 3 + i] += row.mulInner(Vector<f32, 3>{x, y, z});
		}
		return *this;
	}

	Matrix& translateInPlace(const Vector<f32, 3>& vector) {
		Matrix n = Matrix();
		for (usize i = 0; i < Width; ++i) {
			Vector<f32, 4> row4 = operator[](i);
			Vector<f32, 3> row{ row4[0], row4[1], row4[2] };
			m_array[Width * 3 + i] += row.mulInner(vector);
		}
		return *this;
	}

	Matrix& rotate(f32 x, f32 y, f32 z) {
		f32 sin = std::sin(x);
		f32 cos = std::cos(x);

		m_array[Width * 1 + 1] = cos;
		m_array[Width * 1 + 2] = sin;
		m_array[Width * 2 + 1] = -sin;
		m_array[Width * 2 + 2] = cos;

		sin = std::sin(y);
		cos = std::cos(y);

		m_array[Width * 0 + 0] = cos;
		m_array[Width * 0 + 2] = -sin;
		m_array[Width * 2 + 0] = sin;
		m_array[Width * 2 + 2] = cos;

		sin = std::sin(z);
		cos = std::cos(z);

		m_array[Width * 0 + 0] = cos;
		m_array[Width * 0 + 1] = sin;
		m_array[Width * 1 + 0] = -sin;
		m_array[Width * 1 + 1] = cos;
		return *this;
	}

	Matrix& rotate(const Vector<f32, 3>& vector) {
		f32 sin = std::sin(vector[0]);
		f32 cos = std::cos(vector[0]);

		m_array[Width * 1 + 1] = cos;
		m_array[Width * 1 + 2] = sin;
		m_array[Width * 2 + 1] = -sin;
		m_array[Width * 2 + 2] = cos;

		sin = std::sin(vector[1]);
		cos = std::cos(vector[1]);

		m_array[Width * 0 + 0] = cos;
		m_array[Width * 0 + 2] = -sin;
		m_array[Width * 2 + 0] = sin;
		m_array[Width * 2 + 2] = cos;

		sin = std::sin(vector[2]);
		cos = std::cos(vector[2]);

		m_array[Width * 0 + 0] = cos;
		m_array[Width * 0 + 1] = sin;
		m_array[Width * 1 + 0] = -sin;
		m_array[Width * 1 + 1] = cos;
		return *this;
	}

	Matrix& scale(f32 x, f32 y, f32 z) {
		for (usize r = 0; r < Height; ++r) {
			for (usize c = 0; c < Width; ++c) {
				f32 s = 1;
				if (r == 0) {
					s = x;
				} else if (r == 1) {
					s = y;
				} else if (r == 2) {
					s = z;
				}

				m_array[c + r * Width] *= s;
			}

		}

		return *this;
	}

	Matrix& scale(const Vector<f32, 3>& vector) {
		for (usize r = 0; r < Height; ++r) {
			for (usize c = 0; c < Width; ++c) {
				f32 s = 1;
				if (r == 0) {
					s = vector[0];
				} else if (r == 1) {
					s = vector[1];
				} else if (r == 2) {
					s = vector[2];
				}

				m_array[c + r * Width] *= s;
			}

		}

		return *this;
	}

	Matrix& perspective(f32 fov, f32 aspect, f32 near, f32 far) {
		const f32 a = 1 / std::tan(fov / 2);

		m_array[0] = a / aspect;
		m_array[Width + 1] = a;
		m_array[Width * 2 + 2] = -((far + near) / (far - near));
		m_array[Width * 2 + 3] = -1.0f;
		m_array[Width * 3 + 2] = -((2 * far * near) / (far - near));
		return *this;
	}

	Matrix& orthographic(f32 left, f32 right, f32 bottom, f32 top, f32 near, f32 far) {
		m_array[0] = 2 / (right - left);
		m_array[Width + 1] = 2 / (top - bottom);
		m_array[Width * 2 + 2] = -2 / (far - near);
		m_array[Width * 3] = -(right + left) / (right - left);
		m_array[Width * 3 + 1] = -(top + bottom) / (top - bottom);
		m_array[Width * 3 + 2] = -(far + near) / (far - near);
		m_array[Width * 3 + 3] = 1.0f;
		return *this;
	}

	Matrix operator+(const Matrix& matrix) {
		Matrix n = Matrix();
		for (usize r = 0; r < Height; ++r) {
			for (usize c = 0; c < Width; ++c) {
				n.m_array[c + r * Width] = m_array[c + r * Width] + matrix.m_array[c + r * Width];
			}
		}

		return n;
	}

	Matrix operator-(const Matrix& matrix) {
		Matrix n = Matrix();
		for (usize r = 0; r < Height; ++r) {
			for (usize c = 0; c < Width; ++c) {
				n.m_array[c + r * Width] = m_array[c + r * Width] - matrix.m_array[c + r * Width];
			}
		}

		return n;
	}

	Matrix operator*(const Matrix& matrix) {
		Matrix n = Matrix();
		for (usize c = 0; c < Width; ++c) {
			for (usize r = 0; r < Height; ++r) {
				for (usize k = 0; k < Width; ++k) {
					n.m_array[c + r * Width] += m_array[k + r * Width] * matrix.m_array[c + k * Width];
				}
			}
		}

		return n;
	}

	Matrix& operator=(const Matrix& matrix) {
		for (usize i = 0; i < Width * Height; ++i) {
			m_array[i] = matrix.m_array[i];
		}

		return *this;
	}

	Vector<f32, Width> operator[](usize row) {
		if (row > Height) {
			throw std::out_of_range("Row index is out of range of Matrix bounds");
		}

		Vector<f32, Width> v;
		for (usize i = 0; i < Width; ++i) {
			v[i] = m_array[i + row * Width];
		}
		return v;
	}

	constexpr f32* data() noexcept { return m_array.data(); }

private:
	std::array<f32, Width * Height> m_array;
};

} // namespace kengine::util

#endif
