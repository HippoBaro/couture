//
// Created by Hippolyte Barraud on 14/01/2018.
//

#ifndef DIRECTORY_TEST_MATRIX_HPP
#define DIRECTORY_TEST_MATRIX_HPP

#include <boost/simd.hpp>
#include <boost/simd/memory/allocator.hpp>
#include <vector>
#include <algorithm>

template <typename T>
class matrix {
public:
    using node = T;
public:
    // This class either handle it's own memory or borrow's someone else's (useful for zero-copy interoperability
    // with OpenCV's matrices
    std::vector<T, boost::simd::allocator<T>> flat_buffer;
    T* data;

    int height;
    int width;
    int o_width;

    matrix(int height, int width, T* data) : flat_buffer(), data(data), height(height), width(width), o_width(width) {}
    matrix(int height, int width) : flat_buffer(height * width), data(flat_buffer.data()), height(height), width(width), o_width(width) {}

    // We enforce no copy of this class, since it's very heavy.
    matrix(matrix const &other) = delete;

    matrix &operator=(matrix const &other) = delete;

    // Moving it around is okay
    matrix(matrix &&other) noexcept : flat_buffer(std::move(other.flat_buffer)),
                                      height(other.height), width(other.width), o_width(width) {};

    matrix &operator=(matrix &&other) noexcept {
        flat_buffer = std::move(other.flat_buffer);
        height = other.height;
        width = other.width;
        o_width = other.o_width;
        return *this;
    }

    // Here we create the artificial 2-dimentional representation of the matrix. OpenCV uses the same approach
    inline T at(int y, int x) const { return data[y * o_width + x]; }

    inline T &at(int y, int x) { return data[y * o_width + x]; }

    inline node const *row_at(int y) const { return &data[y * o_width]; };

    inline node *row_at(int y) { return &data[y * o_width]; };

    // Calling vector::erase would be very expensive so we allow blanks in the dataset.
    // We can live with then since we control what the user's see.
    inline void erase(int y, int x) {
        std::rotate(data + (y * o_width + x), data + (y * o_width + x) + 1,
                    data + ((y + 1) * o_width + 0));
    }
};

//

#endif //DIRECTORY_TEST_MATRIX_HPP
