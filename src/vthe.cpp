#include "vthe.h"
#include "seal/seal.h"
#include "utils.h"

class Vthe::Impl {
public:
  Impl(const seal::EncryptionParameters &params) : ctx(seal::SEALContext::Create(params)) {
    const seal::KeyGenerator keygen(ctx);
    public_key = keygen.public_key();
    secret_key = keygen.secret_key();
    seal::BatchEncoder encoder(ctx);
    const size_t batch_size = encoder.slot_count() / 2;
    transformed = std::vector<uint64_t>();
    transformed.reserve(batch_size);
  }

  std::unique_ptr<std::vector<seal::Ciphertext>> encrypt(const std::vector<bool> &state) {
    const seal::Encryptor encryptor(ctx, public_key);
    seal::BatchEncoder encoder(ctx);
    const size_t batch_size = encoder.slot_count() / 2;
    const size_t num_batches = (state.size() + batch_size - 1) / batch_size;

    std::unique_ptr<std::vector<seal::Ciphertext>> res;
    res->reserve(num_batches);

    for (size_t i = 0; i < num_batches; i++) {
      seal::Ciphertext ct;
      const size_t start = i * batch_size;
      const size_t end = std::min(state.size(), start + batch_size);
      for (size_t j = 0; j < end; j++) {
        transformed[j] = state[start + j] ? 0 : 1;
      }
      encrypter_impl(encryptor, encoder, transformed, ct);
    }

    return res;
  }

private:
  const std::shared_ptr<seal::SEALContext> ctx;
  seal::PublicKey public_key;
  seal::SecretKey secret_key;
  std::vector<uint64_t> transformed;

  void encrypter_impl(const seal::Encryptor &encryptor, seal::BatchEncoder &encoder, gsl::span<const uint64_t> view,
                      seal::Ciphertext &ct) const {
    seal::Plaintext pt;
    encoder.encode(view, pt);
    encryptor.encrypt(pt, ct);
  }
};

Vthe::Vthe(std::unique_ptr<std::vector<bool>> initial_state, const double infectivity_prob,
           const seal::EncryptionParameters &params)
    : infectivity(generate_random_vector(infectivity_prob, state->size())),
      pimpl(std::make_unique<Impl>(Impl(params))) {
  state = std::move(initial_state);
}

const std::vector<bool> &Vthe::get_state() const { return *state; }

const std::vector<bool> &Vthe::get_infectivity() const { return *infectivity; }