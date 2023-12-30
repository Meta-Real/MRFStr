cmake_minimum_required(VERSION 3.20)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED True)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_C_FLAGS "-Wall -Wextra -Werror -Wpedantic -march=native")
set(CMAKE_C_FLAGS_DEBUG "-g")
set(CMAKE_C_FLAGS_RELEASE "-O3")

project(MRFStr VERSION 1.0.0)

add_library(MRFStr STATIC
    fsrcs/memcpy.c fsrcs/memcmp.c fsrcs/memchr.c fsrcs/memset.c
    fsrcs/init.c fsrcs/set.c
    fsrcs/concat.c fsrcs/repeat.c
    fsrcs/reverse.c
    fsrcs/replace.c
    fsrcs/cmp.c fsrcs/contain.c fsrcs/find.c
    fsrcs/io.c)
target_include_directories(MRFStr PUBLIC "heads" PUBLIC ".")

add_custom_command(TARGET MRFStr POST_BUILD COMMAND "${CMAKE_COMMAND}" -E copy libMRFStr.a ..)

install(TARGETS MRFStr DESTINATION lib)
install(FILES mrfstr.h DESTINATION include)
