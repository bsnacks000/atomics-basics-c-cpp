CPMAddPackage(
    NAME catch2
    GIT_TAG v3.6.0
    GITHUB_REPOSITORY catchorg/catch2
    SOURCE_DIR ${LIB_DIR}/catch2
)

add_executable(tests smoke.cpp)
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)
target_include_directories(tests PRIVATE "${CMAKE_SOURCE_DIR}")
target_compile_options(tests PRIVATE -Wall -Wextra -Wpedantic -Werror)

list(APPEND CMAKE_MODULE_PATH ${LIB_DIR}/catch2/extras)
include(CTest)
include(Catch)
catch_discover_tests(tests)