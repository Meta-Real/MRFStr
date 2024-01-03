# MRFStr Library version 0.1.0
# MetaReal Fast String Library

cmake_minimum_required(VERSION 3.20)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

project(MRFStr VERSION 0.1.0 LANGUAGES C)

if (CMAKE_C_COMPILER_ID STREQUAL "GNU" OR CMAKE_C_COMPILER_ID STREQUAL "Clang")
    set(CMAKE_C_FLAGS "-Wall -Wextra -Werror -Wpedantic -march=native")
    set(CMAKE_C_FLAGS_DEBUG "-g")
    set(CMAKE_C_FLAGS_RELEASE "-O3")
elseif (MSVC)
    set(CMAKE_C_FLAGS "/Wall /wd5045 /wd5105 /wd4324 /wd4820 /wd4710 /Zi /WX")
    set(CMAKE_C_FLAGS_DEBUG "/Od")
    set(CMAKE_C_FLAGS_RELEASE "/O2")

    try_run(_ _ "${PROJECT_SOURCE_DIR}/msvcsetup"
        "${PROJECT_SOURCE_DIR}/msvcsetup.c"
        WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}")
    file(REMOVE_RECURSE "${PROJECT_SOURCE_DIR}/msvcsetup")
endif()

add_library(MRFStr STATIC
    srcs/memcpy.c srcs/memset.c srcs/memcmp.c srcs/memchr.c
    srcs/init.c srcs/set.c
    srcs/concat.c srcs/repeat.c
    srcs/reverse.c srcs/replace.c
    srcs/cmp.c srcs/contain.c srcs/find.c
    srcs/io.c)
target_include_directories(MRFStr PUBLIC "heads" PUBLIC ".")
target_compile_definitions(MRFStr PRIVATE $<$<CONFIG:Debug>:__MRFSTR_DEBUG__>)

enable_testing()
add_subdirectory(tests)

set_target_properties(MRFStr PROPERTIES PUBLIC_HEADER mrfstr.h)
install(TARGETS MRFStr
    LIBRARY DESTINATION lib
    PUBLIC_HEADER DESTINATION include)
