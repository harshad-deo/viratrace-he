#include "bench_utils.h"
#include "utils.h"
#include "vthe.h"
#include <benchmark/benchmark.h>
#include <seal/seal.h>

static const size_t RANGE_MIN = 1;
static const size_t RANGE_MAX = 1 << 4;
static const size_t RANGE_MULT = 2;

class DecryptBuilder : public benchmark::Fixture {
public:
  void SetUp(const ::benchmark::State &state) {
    if (state.thread_index == 0) {
      cts = std::vector<std::unique_ptr<std::vector<seal::Ciphertext>>>();
      size_t range = RANGE_MIN;
      while (range <= RANGE_MAX) {
        vthe_decrypt_builder(range);
        range *= RANGE_MULT;
      }
    }
  }

  std::vector<std::unique_ptr<std::vector<seal::Ciphertext>>> cts;
  std::vector<std::unique_ptr<Vthe>> vthes;

private:
  void vthe_decrypt_builder(const size_t sz) {
    const size_t poly_modulus_degree = 8192;
    seal::EncryptionParameters params(seal::scheme_type::BFV);

    params.set_poly_modulus_degree(poly_modulus_degree);
    params.set_coeff_modulus(seal::CoeffModulus::BFVDefault(poly_modulus_degree));
    params.set_plain_modulus(seal::PlainModulus::Batching(poly_modulus_degree, 20));

    const auto ctx = seal::SEALContext::Create(params);
    seal::BatchEncoder encoder(ctx);
    const size_t batch_size = encoder.slot_count() / 2;
    const size_t simulation_size = batch_size * sz;

    const double prior_p_alice = 0.5;
    auto prior_alice = generate_random_vector(prior_p_alice, simulation_size);

    const double prior_p_bob = 0.3;
    auto prior_bob = generate_random_vector(prior_p_bob, simulation_size);

    const double infectivity_alice = 0.4;
    const double infectivity_bob = 0.5;

    Vthe alice(prior_alice, infectivity_alice, params);
    Vthe bob(prior_bob, infectivity_bob, params);

    const double infectivity_likelihood = 0.3;
    const auto likelihood = generate_random_vector(infectivity_likelihood, simulation_size);

    auto enc = alice.encrypt_state();
    bob.multiply_inplace(*enc);
    cts.emplace_back(std::move(enc));
    vthes.emplace_back(std::make_unique<Vthe>(std::move(alice)));
  }
};

BENCHMARK_DEFINE_F(DecryptBuilder, BM_decrypt_8192)(benchmark::State &state) {
  for (auto _ : state) {
    const int idx = log2Int(state.range(0) / RANGE_MIN) / log2Int(RANGE_MULT);
    auto &ct = cts[idx];
    auto &vthe = vthes[idx];
    (*vthe).decrypt_and_update(*ct);
    benchmark::ClobberMemory();
  }
}

BENCHMARK_REGISTER_F(DecryptBuilder, BM_decrypt_8192)
    ->RangeMultiplier(RANGE_MULT)
    ->Range(RANGE_MIN, RANGE_MAX)
    ->MinTime(3)
    ->Unit(benchmark::kMillisecond);

BENCHMARK_MAIN();