# couture

![](https://i.imgur.com/d4xM3Hg.gif)

Couture is a fast implementation of the seam-carving algorithm with heavy use of vectorization such as SSE and AVX. It uses Boost.Simd internally.

## Usage

**GUI**

The GUI need an image file as an input:

```
$ ./couture_gui test.jpg
```

**CLI**

The CLI takes a single argument, the input file

```
$ ./couture 4-in.txt
```

It will print the output on the standard output.

> Note: The standard output (on Windows especially) is very slow. It is recommended to pipe the output of the program inside a file for maximum performance.

```
$ ./couture 4-in.txt > 4-out.txt
```

## Requirement

### Compiler & Toolchain
- Clang 4 or later
- GCC 5 or later
- CMake 3.4 or later

> The code hasn't been tested on Windows but should work on Visual Studio 2015 and later, or Clang 5

### Dependency

**Common**

These are required for both the GUI and CLI
- Boost 1.60 or later
- Boost.Simd (included in the repo as a submodule)

**Gui**
- OpenCV 3 and later

## Build

### Cloning

```
$ git clone https://github.com/HippoBaro/couture.git --recursive
```

### Building

```
$ cd couture && mkdir build && cd build
$ cmake ..
$ make
```

> The procedure will be different on Windows as you have to generate the proper SLN files via CMake and then open the solution with Visual Studio to build the projects.