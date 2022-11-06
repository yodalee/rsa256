#include "verilog_int.h"
#include <gmp.h>

constexpr unsigned kBW_RSA = 256;
typedef verilog::vuint<kBW_RSA+2> rsa_key_t;

// calculate ((2 ** power) mod n)
void two_power_mod(mpz_t out, const unsigned power, const mpz_t N);
void two_power_mod(rsa_key_t &out, const unsigned power, const rsa_key_t &N);

// montgomery algorithm
// Given two numbers A and B, and the mod number N
// calculate (A * B * (2 ** -256)) mod N, 1 bit per round
void montgomery_base2(mpz_t out, const mpz_t A, const mpz_t B, const mpz_t N);
void montgomery_base2(rsa_key_t &out, const rsa_key_t &A, const rsa_key_t &B, const rsa_key_t &N);

// modular_exponentiation by least-significant-bit (LSB) multiplication
// Given two numbers A and B, and the mod number N
// calculate  A ** B mod N
void lsb_modular_exponentiation(mpz_t out, const mpz_t A, const mpz_t B, const mpz_t N);
void lsb_modular_exponentiation(rsa_key_t &out, const rsa_key_t &A, const rsa_key_t &B, const rsa_key_t &N);

// rsa
// Given two number msg and key and the mod number N
// calculate  msg ** key mod N
void rsa(mpz_t crypto, const mpz_t msg, const mpz_t key, const mpz_t N);
void rsa(rsa_key_t &crypto, const rsa_key_t &msg, const rsa_key_t &key, const rsa_key_t &N);
