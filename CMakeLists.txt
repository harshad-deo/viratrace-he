cmake_minimum_required(VERSION 3.15)

project(ViraTrace-HE)

find_package(SEAL 3.5 REQUIRED)
find_package(Microsoft.GSL CONFIG)

add_compile_options("-Wall")
add_compile_options("-O3")
include_directories("include")

add_executable(vthe
  src/vthe.cpp
  )

target_link_libraries(vthe SEAL::seal)
target_link_libraries(vthe Microsoft.GSL::GSL)

set_property(TARGET vthe PROPERTY CXX_STANDARD 17)
