cmake_minimum_required(VERSION 3.20)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED True)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

project(MRFStr VERSION 0.1.0)

if (CMAKE_C_COMPILER_ID STREQUAL "GNU" OR CMAKE_C_COMPILER_ID STREQUAL "Clang")
    set(CMAKE_C_FLAGS "-Wall -Wextra -Werror -Wpedantic -march=native")
    set(CMAKE_C_FLAGS_DEBUG "-g")
    set(CMAKE_C_FLAGS_RELEASE "-O3")
elseif (CMAKE_C_COMPILER_ID STREQUAL "MSVC")
    set(CMAKE_C_FLAGS "/Wall /wd5045 /wd5105 /wd4324 /wd4820 /wd4710 /Zi /WX")
    set(CMAKE_C_FLAGS_DEBUG "/Od")
    set(CMAKE_C_FLAGS_RELEASE "/O2")

    try_run(_ _ "${PROJECT_SOURCE_DIR}/winsetup"
        "${PROJECT_SOURCE_DIR}/winsetup.c"
        WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}")
endif()

add_library(MRFStr STATIC
    fsrcs/memcpy.c fsrcs/memset.c fsrcs/memcmp.c fsrcs/memchr.c
    fsrcs/init.c fsrcs/set.c
    fsrcs/concat.c fsrcs/repeat.c
    fsrcs/reverse.c fsrcs/replace.c
    fsrcs/cmp.c fsrcs/contain.c fsrcs/find.c
    fsrcs/io.c)
target_include_directories(MRFStr PUBLIC "heads" PUBLIC ".")

add_executable(Test test.c)
target_link_libraries(Test MRFStr)
add_test(NAME Test COMMAND Test)

install(TARGETS MRFStr DESTINATION lib)
install(FILES mrfstr.h DESTINATION include)
