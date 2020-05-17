#include <benchmark/benchmark.h>

static void BM_empty(benchmark::State &state) {
  for (auto _ : state) {
  }
}

BENCHMARK(BM_empty);

BENCHMARK_MAIN();