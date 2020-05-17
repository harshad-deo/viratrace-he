#include <memory>
#include <seal/seal.h>
#include <vector>

#ifndef VTHE
#define VTHE

/**
 * Infectivity is assumed to be sampled from a constant binomial
 */
class Vthe {
public:
  Vthe(std::vector<bool> &initial_state, const double infectivity_prob, const seal::EncryptionParameters &params);
  ~Vthe();
  Vthe(Vthe &&);
  Vthe &operator=(Vthe &&);

  const std::vector<bool> &get_state() const;
  const std::vector<bool> &get_infectivity() const;

  std::vector<seal::Ciphertext> encrypt_state();

  // the cipher texts are modified
  void multiply(std::vector<seal::Ciphertext> &cts);

  void decrypt_and_update(std::vector<seal::Ciphertext> &cts);

private:
  std::vector<bool> state;
  std::vector<bool> infectivity;
  std::vector<bool> likelihood;
  class Impl;
  std::unique_ptr<Impl> pimpl;
};

#endif