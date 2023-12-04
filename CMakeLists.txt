cmake_minimum_required(VERSION 3.20)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED True)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_C_FLAGS "-Wall -Wextra -march=native")
set(CMAKE_C_FLAGS_DEBUG "-g")
set(CMAKE_C_FLAGS_RELEASE "-O3")

project(MRFStr VERSION 1.0.0)

add_library(MRFStr STATIC srcs/alloc.c
    fsrcs/memcpy.c fsrcs/memcmp.c fsrcs/memchr.c fsrcs/memset.c
    fsrcs/init.c fsrcs/set.c
    fsrcs/repeat.c
    fsrcs/cmp.c fsrcs/contain.c
    fsrcs/reverse.c
    fsrcs/util.c)
target_include_directories(MRFStr PUBLIC "heads" PUBLIC ".")

add_custom_command(TARGET MRFStr POST_BUILD COMMAND "${CMAKE_COMMAND}" -E copy libMRFStr.a ..)

install(TARGETS MRFStr DESTINATION lib)
install(FILES mrfstr.h DESTINATION include)

project(MRTStr VERSION 1.0.0)

add_library(MRTStr STATIC srcs/alloc.c tsrcs/thread.c
    tsrcs/memcpy.c tsrcs/memcmp.c tsrcs/memchr.c tsrcs/memset.c
    tsrcs/init.c tsrcs/set.c
    tsrcs/repeat.c
    tsrcs/cmp.c tsrcs/contain.c
    tsrcs/util.c)
target_include_directories(MRTStr PUBLIC "heads" PUBLIC ".")

add_custom_command(TARGET MRTStr POST_BUILD COMMAND "${CMAKE_COMMAND}" -E copy libMRTStr.a ..)

install(TARGETS MRTStr DESTINATION lib)
install(FILES mrtstr.h DESTINATION include)
