#include "verilog_int.h"
#include "rsa.h"
#include <iostream>
using namespace std;

void two_power_mod(mpz_t out, const unsigned power, const mpz_t N) {
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

void two_power_mod(rsa_key_t &out, const unsigned power, const rsa_key_t &N) {
  out = 1;
  for (unsigned i = 0; i < power; ++i) {
    out <<= 1;
    if (out >= N) {
      out -= N;
    }
  }
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

void montgomery_base2(rsa_key_t &out, const rsa_key_t &A, const rsa_key_t &B, const rsa_key_t &N) {
  out = 0;
  for (int i = 0; i < 256; ++i) {
    if (A.Bit(i)) {
      out += B;
    }
    if (out.Bit(0)) {
      out += N;
    }
    out >>= 1;
  }
  if (out > N) {
    out -= N;
  }
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

void lsb_modular_exponentiation(rsa_key_t &out, const rsa_key_t &A, const rsa_key_t &B, const rsa_key_t &N) {
  int src_idx = 0;
  // use double buffer, each loop we use src_idx to compute dst_idx
  rsa_key_t square[2], multiple[2];
  multiple[src_idx] = 1;
  square[src_idx] = A;
  for (int i = 0; i < 256; ++i) {
    const int dst_idx = 1 - src_idx;
    if (B.Bit(i)) {
      montgomery_base2(multiple[dst_idx], multiple[src_idx], square[src_idx], N);
    } else {
      multiple[dst_idx] = multiple[src_idx];
    }
    montgomery_base2(square[dst_idx], square[src_idx], square[src_idx], N);
    src_idx = 1 - src_idx;
  }
  out = multiple[src_idx];
}

void rsa(mpz_t out, const mpz_t msg, const mpz_t key, const mpz_t N) {
  mpz_t pack_value;
  mpz_t packed_msg;
  mpz_t crypto_msg;
  mpz_inits(pack_value, packed_msg, crypto_msg, NULL);

  two_power_mod(pack_value, 512, N);
  montgomery_base2(packed_msg, msg, pack_value, N);
  lsb_modular_exponentiation(out, packed_msg, key, N);
}

void rsa(rsa_key_t &crypto, const rsa_key_t &msg, const rsa_key_t &key, const rsa_key_t &N) {
  rsa_key_t pack_value;
  rsa_key_t packed_msg;
  two_power_mod(pack_value, 512, N);
  montgomery_base2(packed_msg, msg, pack_value, N);
  lsb_modular_exponentiation(crypto, packed_msg, key, N);
}
