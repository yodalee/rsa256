
#include<gmp.h>

// calculate ((2 ** power) mod n)
void two_power_mod(unsigned power, mpz_t N, mpz_t out);

// montgomery algorithm
// Given two numbers A and B, and the mod number N
// calculate (A * B * (2 ** -256)) mod N, 1 bit per round
void montgomery_base2(mpz_t A, mpz_t B, mpz_t N, mpz_t out);

// modular_exponentiation by least-significant-bit (LSB) multiplication
// Given two numbers A and B, and the mod number N
// calculate  A ** B mod N
void lsb_modular_exponentiation(mpz_t A, mpz_t B, mpz_t N, mpz_t out);

// rsa
// Given two number msg and key and the mod number N
// calculate  msg ** key mod N
void rsa(mpz_t msg, mpz_t key, mpz_t N);
