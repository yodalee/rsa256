#include "rsa.h"

void two_power_mod( mpz_t out, const unsigned power, const mpz_t N) {
  mpz_t base;
  mpz_init_set_ui(base, 1u);

  for (int i = 0; i < power; ++i) {
    mpz_mul_ui(base, base, 2u);
    if (mpz_cmp(base, N) > 0) {
      mpz_sub(base, base, N);
    }
  }
  mpz_set(out, base);
}

void montgomery_base2(mpz_t out, const mpz_t A, const mpz_t B, const mpz_t N) {
  mpz_t round_result; // S in doc
  mpz_init_set_ui(round_result, 0);

  for (int i = 0; i < 256; ++i) {
    bool bit_i = mpz_tstbit(A, i);
    if (bit_i) {
      mpz_add(round_result, round_result, B);
    }
    bool is_odd = mpz_tstbit(round_result, 0);
    if (is_odd) {
      mpz_add(round_result, round_result, N);
    }
    mpz_tdiv_q_ui(round_result, round_result, 2u);
  }
  if (mpz_cmp(round_result, N) > 0) {
    mpz_sub(round_result, round_result, N);
  }
  mpz_set(out, round_result);
}

void lsb_modular_exponentiation(mpz_t out, const mpz_t A, const mpz_t B, const mpz_t N) {
  mpz_t square; // T in doc
  mpz_t multiple; // S in doc
  mpz_init_set(square, A);
  mpz_init_set_ui(multiple, 1u);

  for (int i = 0; i < 256; ++i) {
    bool bit_i = mpz_tstbit(B, i);
    if (bit_i) {
      montgomery_base2(multiple, multiple, square, N);
    }
    montgomery_base2(square, square, square, N);
  }

  mpz_set(out, multiple);
}
