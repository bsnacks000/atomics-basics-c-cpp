cmake_minimum_required(VERSION 3.26)

project(atomics VERSION 0.1.0 DESCRIPTION "generic atomic data structures" LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD_REQUIRED 17)
set(CMAKE_C_STANDARD_REQUIRED 17)

if(NOT CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE STREQUAL "")
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "" FORCE)
endif()

# set(LIB_DIR "${CMAKE_SOURCE_DIR}/libs")
# option(BUILD_TESTS "Build the Tests." ON)

# if(BUILD_TESTS)
# enable_testing()
# include(cmake/get_cpm.cmake)
# add_subdirectory(tests)
# endif()
add_executable(consistency consistency.cpp)
target_compile_options(consistency PRIVATE -Wall -Wpedantic -Werror -Wextra)

add_executable(relaxed relaxed.cpp)
target_compile_options(relaxed PRIVATE -Wall -Wpedantic -Werror -Wextra)

add_executable(relaxed2 relaxed2.cpp)
target_compile_options(relaxed2 PRIVATE -Wall -Wpedantic -Werror -Wextra)

add_executable(acqrel acqrel.cpp)
target_compile_options(acqrel PRIVATE -Wall -Wpedantic -Werror -Wextra)

add_executable(acqrelax acqrelax.cpp)
target_compile_options(acqrelax PRIVATE -Wall -Wpedantic -Werror -Wextra)

add_executable(spinner spinner.c)
target_link_libraries(spinner PRIVATE pthread)
target_compile_options(spinner PRIVATE -Wall -Wpedantic -Werror -Wextra -std=gnu17)

add_executable(ringbuf ringbuf.cpp)
target_compile_options(ringbuf PRIVATE -Wall -Wpedantic -Werror -Wextra -std=c++20)

add_executable(ttas_spinlock ttas_spinlock.cpp)
target_compile_options(ttas_spinlock PRIVATE -Wall -Wpedantic -Werror -Wextra -std=c++20)

add_executable(spin_ringbuf spin_ringbuf.cpp)
target_compile_options(spin_ringbuf PRIVATE -Wall -Wpedantic -Werror -Wextra -std=c++20)