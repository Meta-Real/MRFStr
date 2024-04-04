cmake_minimum_required(VERSION 3.20)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

project(MRFStr VERSION 1.2.0
    DESCRIPTION "MetaReal Fast String library version 1.2.0"
    LANGUAGES C ASM)
option(BUILD_SHARED_LIBS "Shared Library")

if (CMAKE_C_COMPILER_ID STREQUAL "GNU" OR CMAKE_C_COMPILER_ID STREQUAL "Clang")
    set(CMAKE_C_FLAGS "-Wall -Wextra -Werror -Wpedantic")
    set(CMAKE_C_FLAGS_DEBUG "-g")
    set(CMAKE_C_FLAGS_RELEASE "-O3")
    set(CMAKE_C_FLAGS_MINSIZEREL "-Os")
    set(CMAKE_C_FLAGS_RELWITHDEBINFO "-O2 -g")

    enable_language(ASM-ATT)
elseif (MSVC)
    set(CMAKE_C_FLAGS "/Wall /wd4324 /wd4710 /wd4711 /wd4820 /wd4996 /wd5045 /wd5105 /WX")
    set(CMAKE_C_FLAGS_DEBUG "/Od /Zi")
    set(CMAKE_C_FLAGS_RELEASE "/O2")
    set(CMAKE_C_FLAGS_MINSIZEREL "/Os")
    set(CMAKE_C_FLAGS_RELWITHDEBINFO "/O2 /Zi")

    enable_language(ASM_MASM)
endif()

if (BUILD_SHARED_LIBS)
    add_compile_definitions(__MRFSTR_SHARED__)
    add_compile_definitions(__MRFSTR_COMPILE_TIME__)
endif()

add_library(MRFStr
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
    srcs/str.c
    srcs/config.c)
target_include_directories(MRFStr PUBLIC heads PUBLIC .)

if (CMAKE_C_COMPILER_ID STREQUAL "GNU" OR CMAKE_C_COMPILER_ID STREQUAL "Clang")
    target_sources(MRFStr
        PRIVATE core/scopy.S PRIVATE core/sfill.S PRIVATE core/srev.S
        PRIVATE core/sreplchr.S PRIVATE core/sequal.S PRIVATE core/scontchr.S
        PRIVATE core/sfindchr.S PRIVATE core/scountchr.S PRIVATE core/sstrlen.S
        PRIVATE cpuid/cputype.S PRIVATE cpuid/funccnt.S PRIVATE cpuid/simdset.S
        PRIVATE cpuid/proccnt.S PRIVATE cpuid/cachesize.S)
elseif (MSVC)
    target_sources(MRFStr
        PRIVATE core/scopy.asm PRIVATE core/sfill.asm PRIVATE core/srev.asm
        PRIVATE core/sreplchr.asm PRIVATE core/sequal.asm PRIVATE core/scontchr.asm
        PRIVATE core/sfindchr.asm PRIVATE core/scountchr.asm PRIVATE core/sstrlen.asm
        PRIVATE cpuid/cputype.asm PRIVATE cpuid/funccnt.asm PRIVATE cpuid/simdset.asm
        PRIVATE cpuid/proccnt.asm PRIVATE cpuid/cachesize.asm)
endif()

enable_testing()
add_subdirectory(tests)
add_subdirectory(benchmarks)

set_target_properties(MRFStr PROPERTIES PUBLIC_HEADER mrfstr.h)
install(TARGETS MRFStr
    LIBRARY DESTINATION lib
    PUBLIC_HEADER DESTINATION include)
