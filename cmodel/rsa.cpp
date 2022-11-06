#include "verilog_int.h"
#include "rsa.h"
// #include <iostream>
// using namespace std;

void two_power_mod(rsa_key_t &out, const unsigned power, const rsa_key_t &N) {
  out = 1;
  for (unsigned i = 0; i < power; ++i) {
    out <<= 1;
    if (out >= N) {
      out -= N;
    }
  }
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

void rsa(rsa_key_t &crypto, const rsa_key_t &msg, const rsa_key_t &key, const rsa_key_t &N) {
  rsa_key_t pack_value;
  rsa_key_t packed_msg;
  two_power_mod(pack_value, 512, N);
  montgomery_base2(packed_msg, msg, pack_value, N);
  lsb_modular_exponentiation(crypto, packed_msg, key, N);
}
