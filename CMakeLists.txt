cmake_minimum_required(VERSION 3.20)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

project(MRFStr VERSION 1.2.0
    DESCRIPTION "MetaReal Fast String library version 1.2.0"
    LANGUAGES C)
option(BUILD_SHARED_LIBS "Shared Library")

if (CMAKE_C_COMPILER_ID STREQUAL "GNU" OR CMAKE_C_COMPILER_ID STREQUAL "Clang")
    set(CMAKE_C_FLAGS "-Wall -Wextra -Werror -Wpedantic -march=native")
    set(CMAKE_C_FLAGS_DEBUG "-g")
    set(CMAKE_C_FLAGS_RELEASE "-O3")

    enable_language(ASM)
elseif (MSVC)
    set(CMAKE_C_FLAGS "/Wall /wd5045 /wd5105 /wd4324 /wd4820 /wd4710 /wd4711 /wd4996 /Zi /WX")
    set(CMAKE_C_FLAGS_DEBUG "/Od")
    set(CMAKE_C_FLAGS_RELEASE "/O2")

    enable_language(ASM_MASM)

    try_run(RRES CRES ${PROJECT_SOURCE_DIR}/msvcsetup
        SOURCES ${PROJECT_SOURCE_DIR}/msvcsetup.c ${PROJECT_SOURCE_DIR}/cpuid/simdset.asm
        CMAKE_FLAGS -DINCLUDE_DIRECTORIES=${PROJECT_SOURCE_DIR}
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
    if (NOT CRES)
        message("Failed to compile the msvcsetup")
    elseif (RRES)
        message("Failed to run the msvcsetup")
    endif()

    file(REMOVE_RECURSE ${PROJECT_SOURCE_DIR}/msvcsetup)
endif()

if (BUILD_SHARED_LIBS)
    add_compile_definitions(__MRFSTR_SHARED__)
    add_compile_definitions(__MRFSTR_COMPILE_TIME__)
endif()

add_library(MRFStr
    config/config.c config/avx512.c config/avx.c config/sse.c config/base.c config/binary.c
    proc/copy.c proc/move.c proc/fill.c
    proc/reverse.c proc/replchr.c
    proc/equal.c
    proc/contchr.c proc/findchr.c proc/countchr.c
    srcs/init.c srcs/set.c srcs/get.c
    srcs/concat.c srcs/remove.c srcs/repeat.c
    srcs/reverse.c srcs/replace_chr.c
    srcs/equal.c
    srcs/contain.c srcs/find.c srcs/count_chr.c
    srcs/io.c
    srcs/str.c)
target_include_directories(MRFStr PUBLIC heads PUBLIC .)

if (CMAKE_C_COMPILER_ID STREQUAL "GNU" OR CMAKE_C_COMPILER_ID STREQUAL "Clang")
    target_sources(MRFStr
        PRIVATE cpuid/cputype.S
        PRIVATE cpuid/funccnt.S
        PRIVATE cpuid/simdset.S
        PRIVATE cpuid/proccnt.S)
elseif (MSVC)
    target_sources(MRFStr
        PRIVATE cpuid/cputype.asm
        PRIVATE cpuid/funccnt.asm
        PRIVATE cpuid/simdset.asm
        PRIVATE cpuid/proccnt.asm)
endif()

enable_testing()
add_subdirectory(tests)
add_subdirectory(benchmarks)

set_target_properties(MRFStr PROPERTIES PUBLIC_HEADER mrfstr.h)
install(TARGETS MRFStr
    LIBRARY DESTINATION lib
    PUBLIC_HEADER DESTINATION include)
