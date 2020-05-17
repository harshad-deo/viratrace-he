#include "seal/seal.h"
#include "utils.h"
#include "vthe.h"
#include "gtest/gtest.h"
#include <iostream>

void run_test(const size_t simulation_size) {
  const size_t poly_modulus_degree = 8192;
  seal::EncryptionParameters params(seal::scheme_type::BFV);

  params.set_poly_modulus_degree(poly_modulus_degree);
  params.set_coeff_modulus(seal::CoeffModulus::BFVDefault(poly_modulus_degree));
  params.set_plain_modulus(seal::PlainModulus::Batching(poly_modulus_degree, 20));

  const double prior_p_alice = 0.5;
  auto prior_alice = generate_random_vector(prior_p_alice, simulation_size);
  const std::vector<bool> prior_alice_copy(*prior_alice);

  const double prior_p_bob = 0.3;
  auto prior_bob = generate_random_vector(prior_p_bob, simulation_size);
  const std::vector<bool> prior_bob_copy(*prior_bob);

  const double infectivity_alice = 0.4;
  const double infectivity_bob = 0.5;

  Vthe alice(prior_alice, infectivity_alice, params);
  Vthe bob(prior_bob, infectivity_bob, params);

  auto alice_enc = alice.encrypt_state();
  auto bob_enc = bob.encrypt_state();
  alice.multiply(bob_enc);
  bob.multiply(alice_enc);
  alice.decrypt_and_update(alice_enc);
  bob.decrypt_and_update(bob_enc);

  for (size_t i = 0; i < simulation_size; i++) {
    const bool expected_alice = prior_alice_copy[i] || (prior_bob_copy[i] && bob.get_infectivity()[i]);
    const bool actual_alice = alice.get_state()[i];
    ASSERT_EQ(expected_alice, actual_alice);

    const bool expected_bob = prior_bob_copy[i] || (prior_alice_copy[i] && alice.get_infectivity()[i]);
    const bool actual_bob = bob.get_state()[i];
    ASSERT_EQ(expected_bob, actual_bob);
  }
}

TEST(VT_HE, LESS_THAN_BATCH_SIZE) { run_test(1024); }

TEST(VT_HE, MORE_THAN_BATCH_SIZE) { run_test(1024 * 1024); }

int main(int argc, char **argv) {
  std::cout << "Testing chaloo ki jaye..." << std::endl;
  ::testing::InitGoogleTest(&argc, argv);
  const int res = RUN_ALL_TESTS();
  std::cout << "ho gaya" << std::endl;
  return res;
}