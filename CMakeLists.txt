cmake_minimum_required(VERSION 3.9)
project(directory_test)
set(CMAKE_CXX_STANDARD 14)

set(SOURCES main.cpp main.cpp io.hpp matrix.hpp)

add_executable(couture ${SOURCES})

find_package(Boost 1.60 COMPONENTS iostreams REQUIRED)
target_include_directories(couture PUBLIC ${Boost_INCLUDE_DIR} boost.simd/include)

set(OPTS -Wall -Wextra -Werror -march=native -O3)
target_compile_options(couture PUBLIC ${OPTS})

target_link_libraries(couture LINK_PUBLIC ${Boost_LIBRARIES})