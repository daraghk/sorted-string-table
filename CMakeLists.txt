cmake_minimum_required(VERSION 3.22.1)

project(sorted_string_table)

# GoogleTest requires at least C++11
set(CMAKE_CXX_STANDARD 20)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/0e402173c97aea7a00749e825b194bfede4f2e45.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

FetchContent_Declare(fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt.git
  GIT_TAG master
)
FetchContent_MakeAvailable(fmt)

add_executable(app main.cpp)

target_link_libraries(app fmt::fmt)

enable_testing()

add_executable(tests tests.cc)

target_link_libraries(
  tests
  fmt::fmt
  gtest_main
)

include(GoogleTest)
gtest_discover_tests(tests)