#include "rsa.h"

// using python to test generate
// hex(2 ** 256 mod IN1).upper()
#define IN1 "0xE07122F2A4A9E81141ADE518A2CD7574DCB67060B005E24665EF532E0CCA73E1"
#define OUT1 "0X1F8EDD0D5B5617EEBE521AE75D328A8B23498F9F4FFA1DB99A10ACD1F3358C1F"

#include <cassert>
#include <cstring>

int main(int argc, char** argv)
{
  mpz_t N, out;

  mpz_inits(N, out, NULL);
  mpz_set_str(N, IN1, 0);

  two_power_mod(N, out);

  char buf[256];
  size_t len = 0;
  len = gmp_snprintf(buf, 256, "0x%Zx", N);
  assert(strncmp(buf, IN1, len));
  len = gmp_snprintf(buf, 256, "0x%Zx", out);
  assert(strncmp(buf, OUT1, len));
}
