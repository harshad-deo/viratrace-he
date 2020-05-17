#include "utils.h"
#include "vthe.h"
#include <benchmark/benchmark.h>
#include <seal/seal.h>
#include <tuple>

static std::unique_ptr<std::tuple<std::unique_ptr<std::vector<seal::Ciphertext>>, std::unique_ptr<Vthe>>>
vthe_multiply_builder(const size_t sz) {
  const size_t poly_modulus_degree = 8192;
  seal::EncryptionParameters params(seal::scheme_type::BFV);

  params.set_poly_modulus_degree(poly_modulus_degree);
  params.set_coeff_modulus(seal::CoeffModulus::BFVDefault(poly_modulus_degree));
  params.set_plain_modulus(seal::PlainModulus::Batching(poly_modulus_degree, 20));

  const auto ctx = seal::SEALContext::Create(params);
  seal::BatchEncoder encoder(ctx);
  const size_t batch_size = encoder.slot_count() / 2;
  const size_t simulation_size = batch_size; // * state.range(0);

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

  return std::make_unique<std::tuple<std::unique_ptr<std::vector<seal::Ciphertext>>, std::unique_ptr<Vthe>>>(
      std::make_tuple(std::move(enc), std::make_unique<Vthe>(std::move(bob))));
}

static std::vector<std::unique_ptr<std::tuple<std::unique_ptr<std::vector<seal::Ciphertext>>, std::unique_ptr<Vthe>>>>
build_multiply_vec(const size_t cnt) {
  std::vector<std::unique_ptr<std::tuple<std::unique_ptr<std::vector<seal::Ciphertext>>, std::unique_ptr<Vthe>>>> res;
  for (size_t i = 0; i < cnt; i++) {
    // res.emplace_back(vthe_multiply_builder(i));
  }
  return res;
}

static std::vector<std::unique_ptr<std::tuple<std::unique_ptr<std::vector<seal::Ciphertext>>,
std::unique_ptr<Vthe>>>> multiply_vec = build_multiply_vec(4);

static void BM_multiply_8192(benchmark::State &state) {
  for (auto _ : state) {
    // state.PauseTiming();

    // const size_t poly_modulus_degree = 8192;
    // seal::EncryptionParameters params(seal::scheme_type::BFV);

    // params.set_poly_modulus_degree(poly_modulus_degree);
    // params.set_coeff_modulus(seal::CoeffModulus::BFVDefault(poly_modulus_degree));
    // params.set_plain_modulus(seal::PlainModulus::Batching(poly_modulus_degree, 20));

    // const auto ctx = seal::SEALContext::Create(params);
    // seal::BatchEncoder encoder(ctx);
    // const size_t batch_size = encoder.slot_count() / 2;
    // const size_t simulation_size = batch_size; // * state.range(0);

    // const double prior_p_alice = 0.5;
    // auto prior_alice = generate_random_vector(prior_p_alice, simulation_size);

    // const double prior_p_bob = 0.3;
    // auto prior_bob = generate_random_vector(prior_p_bob, simulation_size);

    // const double infectivity_alice = 0.4;
    // const double infectivity_bob = 0.5;

    // Vthe alice(prior_alice, infectivity_alice, params);
    // Vthe bob(prior_bob, infectivity_bob, params);

    // const double infectivity_likelihood = 0.3;
    // const auto likelihood = generate_random_vector(infectivity_likelihood, simulation_size);

    // auto enc = alice.encrypt_state();
    // state.ResumeTiming();

    // bob.multiply(enc);
    benchmark::ClobberMemory();
  }
}

BENCHMARK(BM_multiply_8192); //->RangeMultiplier(2)->Range(1, 16)->MinTime(3)->Unit(benchmark::kMillisecond);

BENCHMARK_MAIN();