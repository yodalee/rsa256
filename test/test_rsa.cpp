#include "rsa.h"
#include <gtest/gtest.h>

#include <cassert>
#include <cstring>

TEST(RsaTest, test_two_power_mod) {
  // using python to test generate
  // hex(2 ** 256 mod IN1).upper()
  const char IN1[] = "0xE07122F2A4A9E81141ADE518A2CD7574DCB67060B005E24665EF532E0CCA73E1";
  const char OUT1[] = "0X1F8EDD0D5B5617EEBE521AE75D328A8B23498F9F4FFA1DB99A10ACD1F3358C1F";

  mpz_t N, out;
  mpz_init_set_str(N, IN1, 0);
  mpz_init(out);

  two_power_mod(N, out);

  char buf[256];
  size_t len = 0;
  len = gmp_snprintf(buf, 256, "0x%Zx", N);
  EXPECT_TRUE(strncmp(buf, IN1, len));
  len = gmp_snprintf(buf, 256, "0x%Zx", out);
  EXPECT_TRUE(strncmp(buf, OUT1, len));
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
