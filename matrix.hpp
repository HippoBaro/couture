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
private:
    std::vector<std::vector<node, boost::simd::allocator<node>>, boost::simd::allocator<std::vector<node, boost::simd::allocator<node>>>> flat_buffer;

public:

    int height;
    int width;

    matrix(int height, int width) : flat_buffer(height, std::vector<node, boost::simd::allocator<node>>(width)), height(height), width(width) { }

    matrix(matrix const& other) = delete;
    matrix(matrix &&other) noexcept : flat_buffer(std::move(other.flat_buffer)),
                                      height(other.height), width(other.width) {};

    matrix &operator=(matrix const& other) = delete;
    matrix &operator=(matrix &&other) noexcept {
        flat_buffer = std::move(other.flat_buffer);
        height = other.height;
        width = other.width;
        return *this;
    }

    inline T at(int y, int x) const { return flat_buffer[y][x]; }
    inline T& at(int y, int x) { return flat_buffer[y][x]; }

    inline std::vector<node, boost::simd::allocator<node>> const& row_at(int y) const { return flat_buffer[y]; };
    inline std::vector<node, boost::simd::allocator<node>>& row_at(int y) { return flat_buffer[y]; };
};

#endif //DIRECTORY_TEST_MATRIX_HPP
