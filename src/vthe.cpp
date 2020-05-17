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
    transformed = std::vector<uint64_t>(batch_size, 0);
  }

  std::unique_ptr<std::vector<seal::Ciphertext>> encrypt(const std::vector<bool> &state) {
    const seal::Encryptor encryptor(ctx, public_key);
    seal::BatchEncoder encoder(ctx);
    const size_t batch_size = encoder.slot_count() / 2;
    const size_t num_batches = (state.size() + batch_size - 1) / batch_size;

    std::vector<seal::Ciphertext> res;
    res.reserve(num_batches);

    for (size_t i = 0; i < num_batches; i++) {
      seal::Ciphertext ct;
      const size_t start = i * batch_size;
      const size_t end = std::min(state.size() - start, batch_size);
      for (size_t j = 0; j < end; j++) {
        transformed[j] = state[start + j] ? 0 : 1;
      }
      encrypter_impl(encryptor, encoder, transformed, ct);
      res.emplace_back(std::move(ct));
    }

    return std::make_unique<std::vector<seal::Ciphertext>>(res);
  }

  void multiply(std::vector<seal::Ciphertext> &cts, const std::vector<bool> &likelihoods) {
    // seal::BatchEncoder encoder(ctx);
    // seal::Evaluator evaluator(ctx);

    // const size_t batch_size = encoder.slot_count() / 2;
    // const size_t num_batches = (likelihoods.size() + batch_size - 1) / batch_size;
    // if (num_batches != cts.size()) {
    //   throw "Number of batches does not match likelihood size";
    // }

    // std::vector<uint64_t> transformed(batch_size, 0);

    // for (size_t i = 0; i < num_batches; i++) {
    //   seal::Ciphertext &ct = cts[i];
    //   const size_t start = i * batch_size;
    //   const size_t end = std::min(likelihoods.size() - start, batch_size);
    //   for (size_t j = 0; j < end; j++) {
    //     transformed[j] = likelihoods[start + j] ? 0 : 1;
    //   }
    //   multiply_impl(evaluator, encoder, ct, transformed);
    // }
  }

  void decrypt(std::vector<seal::Ciphertext> &cts, std::vector<bool> &state) {
    seal::BatchEncoder encoder(ctx);
    seal::Decryptor decryptor(ctx, secret_key);

    const size_t batch_size = encoder.slot_count() / 2;
    const size_t num_batches = (state.size() + batch_size - 1) / batch_size;
    if (num_batches != cts.size()) {
      throw "Number of batches does not match state size";
    }

    for (size_t i = 0; i < num_batches; i++) {
      seal::Ciphertext &ct = cts[i];
      decrypt_impl(decryptor, encoder, ct, transformed);
      const size_t start = i * batch_size;
      const size_t end = std::min(state.size() - start, batch_size);
      for (size_t j = 0; j < end; j++) {
        state[start + j] = state[start + j] || ((transformed[j] == 0LL) ? true : false);
      }
    }
  }

private:
  const std::shared_ptr<seal::SEALContext> ctx;
  seal::PublicKey public_key;
  seal::SecretKey secret_key;
  std::vector<uint64_t> transformed;

  void encrypter_impl(const seal::Encryptor &encryptor, seal::BatchEncoder &encoder, const std::vector<uint64_t> &state,
                      seal::Ciphertext &ct) const {
    seal::Plaintext pt;
    encoder.encode(state, pt);
    encryptor.encrypt(pt, ct);
  }

  void multiply_impl(seal::Evaluator &evaluator, seal::BatchEncoder &encoder, seal::Ciphertext &ct,
                     const std::vector<uint64_t> &likelihood) const {
    seal::Plaintext pt;
    encoder.encode(likelihood, pt);
    evaluator.multiply_plain_inplace(ct, pt);
  }

  void decrypt_impl(seal::Decryptor &decryptor, seal::BatchEncoder &encoder, seal::Ciphertext &ct,
                    std::vector<uint64_t> &target) const {
    seal::Plaintext pt;
    decryptor.decrypt(ct, pt);
    encoder.decode(pt, target);
  }
};

Vthe::Vthe(std::unique_ptr<std::vector<bool>> &initial_state, const double infectivity_prob,
           const seal::EncryptionParameters &params)
    : state(std::move(initial_state)), infectivity(generate_random_vector(infectivity_prob, state->size())),
      pimpl(std::make_unique<Impl>(Impl(params))) {
  likelihood = std::vector<bool>(state->size(), false);
}

std::unique_ptr<std::vector<seal::Ciphertext>> Vthe::encrypt_state() { return pimpl->encrypt(*state); }

void Vthe::multiply(std::vector<seal::Ciphertext> &cts) {
  for (size_t i = 0; i < state->size(); i++) {
    likelihood[i] = (*state)[i] && (*infectivity)[i];
  }
  pimpl->multiply(cts, likelihood);
}

void Vthe::decrypt_and_update(std::vector<seal::Ciphertext> &cts) { pimpl->decrypt(cts, *state); }

const std::vector<bool> &Vthe::get_state() const { return *state; }

const std::vector<bool> &Vthe::get_infectivity() const { return *infectivity; }

Vthe::~Vthe() = default;

Vthe::Vthe(Vthe &&) = default;

Vthe &Vthe::operator=(Vthe &&) = default;