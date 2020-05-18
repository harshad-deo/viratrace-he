#include "bench_utils.h"
#include "utils.h"
#include "vthe.h"
#include <benchmark/benchmark.h>
#include <seal/seal.h>

static const size_t RANGE_MIN = 1;
static const size_t RANGE_MAX = 1 << 4;
static const size_t RANGE_MULT = 2;

class EncodingFixture : public benchmark::Fixture {
public:
  void SetUp(const ::benchmark::State &state) {
    if (state.thread_index == 0) {
      vthes = std::vector<std::unique_ptr<Vthe>>();
      size_t range = RANGE_MIN;
      while (range <= RANGE_MAX) {
        vthe_enc_builder(range);
        range *= RANGE_MULT;
      }
    }
  }

  std::vector<std::unique_ptr<Vthe>> vthes;

private:
  void vthe_enc_builder(const size_t sz) {
    const size_t poly_modulus_degree = 8192;
    seal::EncryptionParameters params(seal::scheme_type::BFV);

    params.set_poly_modulus_degree(poly_modulus_degree);
    params.set_coeff_modulus(seal::CoeffModulus::BFVDefault(poly_modulus_degree));
    params.set_plain_modulus(seal::PlainModulus::Batching(poly_modulus_degree, 20));

    const auto ctx = seal::SEALContext::Create(params);
    seal::BatchEncoder encoder(ctx);
    const size_t batch_size = encoder.slot_count() / 2;
    const size_t simulation_size = batch_size * sz;

    const double prior_p = 0.5;
    auto prior = generate_random_vector(prior_p, simulation_size);
    const double infectivity = 0.4;

    Vthe alice(prior, infectivity, params);
    vthes.emplace_back(std::make_unique<Vthe>(std::move(alice)));
  }
};

BENCHMARK_DEFINE_F(EncodingFixture, BM_encoding_8192)(benchmark::State &state) {
  for (auto _ : state) {
    std::unique_ptr<std::vector<seal::Ciphertext>> cts;
    const int idx = log2Int(state.range(0) / RANGE_MIN) / log2Int(RANGE_MULT);
    auto &vthe = vthes[idx];
    benchmark::DoNotOptimize(cts = std::move((*vthe).encrypt_state()));
    benchmark::ClobberMemory();
  }
}

BENCHMARK_REGISTER_F(EncodingFixture, BM_encoding_8192)
    ->RangeMultiplier(RANGE_MULT)
    ->Range(RANGE_MIN, RANGE_MAX)
    ->Unit(benchmark::kMicrosecond);

BENCHMARK_MAIN();