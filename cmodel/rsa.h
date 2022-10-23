
#include<gmp.h>

// calculate ((2 ** power) mod n)
void two_power_mod(mpz_t out, const unsigned power, const mpz_t N);

// montgomery algorithm
// Given two numbers A and B, and the mod number N
// calculate (A * B * (2 ** -256)) mod N, 1 bit per round
void montgomery_base2(mpz_t out, const mpz_t A, const mpz_t B, const mpz_t N);

// modular_exponentiation by least-significant-bit (LSB) multiplication
// Given two numbers A and B, and the mod number N
// calculate  A ** B mod N
void lsb_modular_exponentiation(mpz_t out, const mpz_t A, const mpz_t B, const mpz_t N);

// rsa
// Given two number msg and key and the mod number N
// calculate  msg ** key mod N
void rsa(mpz_t crypto, const mpz_t msg, const mpz_t key, const mpz_t N);
