#include "seal/seal.h"
#include "utils.h"
#include "vthe.h"
#include "gtest/gtest.h"
#include <iostream>

TEST(VT_HE, LESS_THAN_BATCH_SIZE) {
  const size_t poly_modulus_degree = 8192;
  seal::EncryptionParameters params(seal::scheme_type::BFV);

  params.set_poly_modulus_degree(poly_modulus_degree);
  params.set_coeff_modulus(seal::CoeffModulus::BFVDefault(poly_modulus_degree));
  params.set_plain_modulus(seal::PlainModulus::Batching(poly_modulus_degree, 20));

  const size_t simulation_size = 512;

  const double prior_p_alice = 0.5;
  std::unique_ptr<std::vector<bool>> prior_alice = generate_random_vector(prior_p_alice, simulation_size);
  const std::vector<bool> prior_alice_copy(*prior_alice);

  const double prior_p_bob = 0.3;
  std::unique_ptr<std::vector<bool>> prior_bob = generate_random_vector(prior_p_bob, simulation_size);
  const std::vector<bool> prior_bob_copy(*prior_bob);

  const double infectivity_alice = 0.4;
  const double infectivity_bob = 0.5;

  Vthe alice(prior_alice, infectivity_alice, params);
  alice.boom();
  // Vthe bob(prior_bob, infectivity_bob, params);
}

int main(int argc, char **argv) {
  std::cout << "Testing chaloo ki jaye..." << std::endl;
  ::testing::InitGoogleTest(&argc, argv);
  const int res = RUN_ALL_TESTS();
  std::cout << "ho gaya" << std::endl;
  return res;
}