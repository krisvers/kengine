#ifndef KRISVERS_KENGINGE_UTIL_MATRIX_HPP
#define KRISVERS_KENGINGE_UTIL_MATRIX_HPP

#include <kengine/types.hpp>
#include <kengine/util/vector.hpp>

namespace kengine::util {

template<usize Width, usize Height>
class Matrix {
public:
	Matrix(const std::array<f32, Width * Height>& values) : m_array(values) {}
	Matrix(const std::initializer_list<f32>& list) { std::copy(list.begin(), list.end(), m_array.data()); }
	Matrix(const Matrix& matrix) : m_array(matrix.m_array) {}

	T& operator[](usize index) {
		if (index > Width * Height) {
			throw std::out_of_range("Index is out of range of Matrix bounds");
		}
		return m_array[index];
	}
	T& operator[][](usize row, usize column) {
		if (column > Width) {
			throw std::out_of_range("Column index is out of range of Matrix bounds");
		}
		if (row > Height) {
			throw std::out_of_range("Row index is out of range of Matrix bounds");
		}
		return m_array[column + row * Width];
	}

private:
	std::array<f32, Width * Height> m_array;
};

} // namespace kengine::util

#endif
