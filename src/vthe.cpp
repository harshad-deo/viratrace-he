#include "vthe.h"
#include "seal/seal.h"
#include "utils.h"

class Vthe::Impl {
public:
  Impl(const seal::EncryptionParameters &params) : ctx(seal::SEALContext::Create(params)) {
    const seal::KeyGenerator keygen(ctx);
    public_key = keygen.public_key();
    secret_key = keygen.secret_key();
  }

private:
  const std::shared_ptr<seal::SEALContext> ctx;
  seal::PublicKey public_key;
  seal::SecretKey secret_key;
};

Vthe::Vthe(std::unique_ptr<std::vector<bool>> initial_state, const double infectivity_prob,
           const seal::EncryptionParameters &params)
    : infectivity(generate_random_vector(infectivity_prob, state->size())),
      pimpl(std::make_unique<Impl>(Impl(params))) {
  state = std::move(initial_state);
}

const std::vector<bool> &Vthe::get_state() const { return *state; }

const std::vector<bool> &Vthe::get_infectivity() const { return *infectivity; }