cmake_minimum_required(VERSION 3.15)
project(ViraTrace-HE)

find_package(GTest REQUIRED)
find_package(SEAL 3.5 REQUIRED)
find_package(Microsoft.GSL CONFIG)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
SET(CMAKE_CXX_FLAGS "-Wall -O3 -fPIC -march=native")

link_libraries(SEAL::seal)
link_libraries(Microsoft.GSL::GSL)
add_subdirectory(bench)

include_directories(include)

add_library(impl
  src/vthe.cpp
  src/utils.cpp
)

target_include_directories(impl PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    ${PROJECT_BINARY_DIR}
    PRIVATE src)

# testing
enable_testing()
add_executable(alltest
  test/all_test.cpp
  test/vthe_test.cpp
  test/vthe_inplace_test.cpp
  )
target_link_libraries(alltest impl gtest pthread)
gtest_add_tests(TARGET alltest
				TEST_SUFFIX .noArgs
                TEST_LIST   noArgsTests)

set_tests_properties(${noArgsTests}   PROPERTIES TIMEOUT 10)

export(TARGETS impl FILE FunimplConfig.cmake)
