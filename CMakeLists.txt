cmake_minimum_required(VERSION 3.4)
project(couture_seam_carving)
set(CMAKE_CXX_STANDARD 14)

####### COMMON

find_package(Boost 1.60 COMPONENTS iostreams REQUIRED)
set(OPTS -Wall -Wextra -Werror -march=native -O3)
set(SOURCES_COMMON seam_carver.hpp matrix.hpp)

####### CLI

set(SOURCES main.cpp io.hpp)
add_executable(couture ${SOURCES_COMMON} ${SOURCES})
target_include_directories(couture PUBLIC ${Boost_INCLUDE_DIR} boost.simd/include)
target_compile_options(couture PUBLIC ${OPTS})
target_link_libraries(couture LINK_PUBLIC ${Boost_LIBRARIES})

####### GUI

find_package(OpenCV REQUIRED )

set(SOURCES_GUI main_gui.cpp)
add_executable(couture_gui ${SOURCES_COMMON} ${SOURCES_GUI})
target_include_directories(couture_gui PUBLIC ${Boost_INCLUDE_DIR} boost.simd/include ${OpenCV_INCLUDE_DIRS})
target_compile_options(couture_gui PUBLIC ${OPTS})
target_link_libraries(couture_gui LINK_PUBLIC ${Boost_LIBRARIES} ${OpenCV_LIBS})