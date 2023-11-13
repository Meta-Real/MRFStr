cmake_minimum_required(VERSION 3.20)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_C_FLAGS "-Wall -Wextra -mavx512f")
set(CMAKE_C_FLAGS_DEBUG "-g")
set(CMAKE_C_FLAGS_RELEASE "-O3")

project(MRFStr VERSION 1.0.0)

add_library(MRFStr STATIC srcs/alloc.c ssrcs/init.c ssrcs/set.c ssrcs/cmp.c)
target_include_directories(MRFStr PUBLIC "heads" PUBLIC ".")

add_custom_command(TARGET MRFStr POST_BUILD COMMAND "${CMAKE_COMMAND}" -E copy libMRFStr.a ..)

project(MRTStr VERSION 1.0.0)

add_library(MRTStr STATIC srcs/alloc.c tsrcs/thread.c tsrcs/init.c tsrcs/set.c tsrcs/cmp.c)
target_include_directories(MRTStr PUBLIC "heads" PUBLIC ".")

add_custom_command(TARGET MRTStr POST_BUILD COMMAND "${CMAKE_COMMAND}" -E copy libMRTStr.a ..)
