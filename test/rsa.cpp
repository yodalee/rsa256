#include "rsa.h"

void two_power_mod(mpz_t N, mpz_t out) {
  mpz_t base;
  mpz_inits(base, NULL);
  mpz_set_ui(base, 1u);

  for (int i = 0; i < 256; ++i) {
    mpz_mul_ui(base, base, 2u);
    if (mpz_cmp(base, N) > 0) {
      mpz_sub(base, base, N);
    }
  }
  mpz_set(out, base);
}
