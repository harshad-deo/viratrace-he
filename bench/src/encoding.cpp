#include "utils.h"
#include "vthe.h"
#include <benchmark/benchmark.h>
#include <seal/seal.h>

static void BM_encoding_8192(benchmark::State &state) {
  for (auto _ : state) {
    state.PauseTiming();

    const size_t poly_modulus_degree = 8192;
    seal::EncryptionParameters params(seal::scheme_type::BFV);

    params.set_poly_modulus_degree(poly_modulus_degree);
    params.set_coeff_modulus(seal::CoeffModulus::BFVDefault(poly_modulus_degree));
    params.set_plain_modulus(seal::PlainModulus::Batching(poly_modulus_degree, 20));

    const auto ctx = seal::SEALContext::Create(params);
    seal::BatchEncoder encoder(ctx);
    const size_t batch_size = encoder.slot_count() / 2;
    const size_t simulation_size = batch_size * state.range(0);

    const double prior_p = 0.5;
    auto prior = generate_random_vector(prior_p, simulation_size);
    const double infectivity = 0.4;

    Vthe alice(prior, infectivity, params);

    std::unique_ptr<std::vector<seal::Ciphertext>> cts;
    state.ResumeTiming();

    benchmark::DoNotOptimize(cts = alice.encrypt_state());
    benchmark::ClobberMemory();
  }
}

BENCHMARK(BM_encoding_8192)->RangeMultiplier(2)->Range(1, 16)->MinTime(3)->Unit(benchmark::kMillisecond);

BENCHMARK_MAIN();