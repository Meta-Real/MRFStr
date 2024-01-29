# MRFStr Library version 1.2.0

MRFStr, short for MetaReal Fast String Library, is a high-performance C library designed to significantly improve the speed of various string operations, including search, copy, compare, and more. \
This library stands out by harnessing the power of SIMD (Single Instruction, Multiple Data) instructions such as AVX512, AVX, and SSE that ensures optimal performance on modern processors.

## Key Features

1. **SIMD Optimization:** MRFStr takes full advantage of SIMD instructions, enabling parallel processing of multiple data elements in a single instruction. This approach boosts the performance of string operations by allowing simultaneous processing of several characters.
2. **Multithreading Support:** MRFStr acknowledges the importance of parallelism in modern computing environments. With built-in support for multithreading, it maximizes resource utilization on multi-core processors, further enhancing the library's overall efficiency.
3. **Cross-Platform Compatibility:** MRFStr is engineered to be cross-platform, ensuring seamless integration into diverse operating systems. Whether you are developing applications on Linux, Windows, or MacOS, MRFStr guarantees consistent and reliable performance across these platforms.
4. **Compiler Compatibility:** MRFStr is compiler-agnostic, offering support for popular compilers such as GCC, Clang, and MSVC. This flexibility guarantees easy library integration for developers using their preferred development environment.

## Performance Benefits

* **Speed:** By leveraging SIMD instructions, MRFStr accelerates string operations, making them significantly faster in contrast to traditional libraries. This speed improvement is especially noticeable in scenarios involving large datasets or repetitive string manipulations.
* **Efficiency:** The multithreading support enhances the overall efficiency, making MRFStr an ideal choice for applications that demand high throughput and responsiveness. Developers can harness the power of parallelism to optimize performance in multithreaded environments.

## Use Cases

MRFStr is well-suited for a wide range of applications, including but not limited to:

* Data processing applications that involve intensive string manipulation.
* High-performance computing tasks where speed and efficiency are critical.
* Applications with large datasets, such as databases or data analysis tools.

## Getting Started

To incorporate MRFStr into your project, include the library and leverage its optimized string operations. The library's ease of integration, comprehensive platform, and compiler support ensure a smooth development experience.

### Prerequisites

To install MRFStr properly, you need these tools:

1. [CMake](https://cmake.org/)
2. [GCC](https://gcc.gnu.org/), [Clang](https://clang.llvm.org/), or [MSVC](https://visualstudio.microsoft.com/downloads/).

If you are on Windows, we recommend using MSVC because it's more compatible with Windows systems.

### Build The Project

Building steps are identical between different operating systems.
To build the library, clone it from Git Hub.

```bash
$ git clone "https://github.com/Meta-Real/MRFStr.git"
$ cd MRFStr
```

Create a folder for building the project.

```bash
$ mkdir build
$ cd build
```

If you want to build a static library, run the following CMake command.

```bash
$ cmake ..
```

If you want to build a dynamic library, run the following CMake command.

```bash
$ cmake .. -DBUILD_SHARED_LIBS=ON
```

Finally, run the following commands to build, test, and install the library.

```bash
$ cmake --build . --config Release
$ ctest
$ cmake --install .
```

Note that to install the library, you need administrative privileges.
On Unix systems, you can use `sudo`:

```bash
$ sudo --install .
```

On Windows, you can use `runas`:

```bash
> runas /user:Administrator "cmake --install ."
```

Finally, you can include the MRFStr library in your projects. Congratulations!
