//
// Created by Hippolyte Barraud on 14/01/2018.
//

/*
 * This file contains utility functions for very fast I/O on ranges of unsigned integers
 */

#ifndef DIRECTORY_TEST_IO_HPP
#define DIRECTORY_TEST_IO_HPP

#include "matrix.hpp"

template <typename U>
void read_input(matrix<U> &matrix, char const* buf, size_t size) {
    unsigned item = 0;
    size_t t = 0;

    while(buf[++t] != '\n');
    ++t;

    int r = 0, c = 0;
    while (t < size) {
        for (; c < matrix.width && t < size; ++t)
        {
            switch (buf[t]) {
                case '0':
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9':
                    item = (item << 3) + (item << 1) + (buf[t]) - '0';
                    break;
                case '\r':
                case ' ':
                    matrix.at(r, c++) = item;
                    item = 0;
                    break;
                default:
                    break;
            }
        }
        while(!isdigit(buf[++t]) && t < size);
        if (item > 0) {
            matrix.at(r, c) = item;
            item = 0;
        }
        r++;
        if (r >= matrix.height) { return; }
        c = 0;
    }
}

const char digit_pairs[201] = {
        "00010203040506070809"
                "10111213141516171819"
                "20212223242526272829"
                "30313233343536373839"
                "40414243444546474849"
                "50515253545556575859"
                "60616263646566676869"
                "70717273747576777879"
                "80818283848586878889"
                "90919293949596979899"
};

template <typename U>
char* itostr(U val, char *s) {
    if (val == 0) {
        s[0] = '0';
        return ++s;
    }

    int size;
    if (val >= 100) {
        size = 3;
    }
    else {
        if (val >= 10)
            size = 2;
        else
            size = 1;
    }

    char *c = &s[size - 1];
    while (val >= 100) {
        int pos = val % 100;
        val /= 100;
        *(short *) (c - 1) = *(short *) (digit_pairs + 2 * pos);
        c -= 2;
    }
    while (val > 0) {
        *c-- = '0' + (val % 10);
        val /= 10;
    }
    return s + size;
}

template <typename U>
void print_matrix(matrix<U> const& matrix) {
    std::string str(static_cast<size_t>(matrix.height * matrix.width * 7), ' ');
    auto strc = const_cast<char *>(str.data()); //My mom's not looking, so it's okay
    for (int r = 0; r < matrix.height; r++) {
        for (int c = 0; c < matrix.width; c++) {
            strc = itostr<uint8_t>(matrix.at(r, c), strc);
            strc++;
        }
        *(strc++) = '\n';
    }
    *--strc = '\0';
    std::cout << str.data() << std::endl;
}

#endif //DIRECTORY_TEST_IO_HPP
