#include "rsa.h"
#include "verilog/dtype/vint.h"
// #include <iostream>
// using namespace std;

void twopower(rsa_key_t &out, const unsigned power, const rsa_key_t &N) {
  using extend_key_t = verilog::vuint<kBW_RSA + 1>;
  extend_key_t base{1};
  extend_key_t extend_N = static_cast<extend_key_t>(N);
  for (unsigned i = 0; i < power; ++i) {
    base <<= 1;
    if (base >= extend_N) {
      base -= extend_N;
    }
  }
  out = static_cast<rsa_key_t>(base);
}

void montgomery_base2(rsa_key_t &out, const rsa_key_t &A, const rsa_key_t &B,
                      const rsa_key_t &N) {
  using extend_key_t = verilog::vuint<kBW_RSA + 2>;
  extend_key_t round{0};
  const extend_key_t extend_B = static_cast<extend_key_t>(B);
  const extend_key_t extend_N = static_cast<extend_key_t>(N);
  for (int i = 0; i < 256; ++i) {
    if (A.Bit(i)) {
      round += extend_B;
    }
    if (round.Bit(0)) {
      round += extend_N;
    }
    round >>= 1;
  }
  if (round > extend_N) {
    round -= extend_N;
  }
  out = static_cast<rsa_key_t>(round);
}

void montgomery_base4(rsa_key_t &out, const rsa_key_t &A, const rsa_key_t &B,
                      const rsa_key_t &N) {
  using extend_key_t = verilog::vuint<kBW_RSA + 3>;
  extend_key_t round{0};
  const extend_key_t extend_B = static_cast<extend_key_t>(B);
  extend_key_t extend_N = static_cast<extend_key_t>(N);
  for (int i = 0; i < 128; ++i) {
    if (A.Bit(i * 2)) {
      round += extend_B;
    }
    if (A.Bit(i * 2 + 1)) {
      round += (extend_B << 1);
    }

    // Make round mod 4 == 0
    if (round.Bit(0)) {
      round += extend_N;
    }
    if (round.Bit(1)) {
      round += (extend_N << 1);
    }
    round >>= 2;
    if (round > extend_N) {
      round -= extend_N;
    }
  }
  out = static_cast<rsa_key_t>(round);
}

void lsb_modular_exponentiation(rsa_key_t &out, const rsa_key_t &A,
                                const rsa_key_t &B, const rsa_key_t &N) {
  int src_idx = 0;
  // use double buffer, each loop we use src_idx to compute dst_idx
  rsa_key_t square[2], multiple[2];
  multiple[src_idx] = 1;
  square[src_idx] = A;
  for (int i = 0; i < 256; ++i) {
    const int dst_idx = 1 - src_idx;
    if (B.Bit(i)) {
      montgomery_base2(multiple[dst_idx], multiple[src_idx], square[src_idx],
                       N);
    } else {
      multiple[dst_idx] = multiple[src_idx];
    }
    montgomery_base2(square[dst_idx], square[src_idx], square[src_idx], N);
    src_idx = 1 - src_idx;
  }
  out = multiple[src_idx];
}

void rsa(rsa_key_t &crypto, const rsa_key_t &msg, const rsa_key_t &key,
         const rsa_key_t &N) {
  rsa_key_t pack_value;
  rsa_key_t packed_msg;
  twopower(pack_value, 512, N);
  montgomery_base4(packed_msg, msg, pack_value, N);
  lsb_modular_exponentiation(crypto, packed_msg, key, N);
}
