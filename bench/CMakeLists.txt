project(bench VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

link_libraries(benchmark)
link_libraries(pthread)

file(GLOB_RECURSE ALL_BENCHMARKS "src/*.cpp")

#building the benchmarks
foreach(ONE_BENCH_CPP ${ALL_BENCHMARKS})
	get_filename_component(ONE_BENCH_EXEC ${ONE_BENCH_CPP} NAME_WE)
  set(TARGET_NAME "${ONE_BENCH_EXEC}")
  add_executable(${TARGET_NAME} ${ONE_BENCH_CPP})
  set_target_properties(${TARGET_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bench")
  target_link_libraries(${TARGET_NAME} impl)
endforeach()