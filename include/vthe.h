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
  Vthe(std::unique_ptr<std::vector<bool>> initial_state, const double infectivity_prob,
       const seal::EncryptionParameters &params);

  const std::vector<bool> &get_state() const;
  const std::vector<bool> &get_infectivity() const;

private:
  std::unique_ptr<std::vector<bool>> state;
  std::unique_ptr<std::vector<bool>> infectivity;
  class Impl;
  const std::unique_ptr<Impl> pimpl;
};

#endif