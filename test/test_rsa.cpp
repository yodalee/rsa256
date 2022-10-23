#include "rsa.h"
#include <gtest/gtest.h>

#include <cassert>
#include <cstring>

TEST(RsaTest, test_two_power_mod) {
  // using python to test generatation
  // hex(2 ** 512 % IN1).upper()
  const char str_N[] = "0xE07122F2A4A9E81141ADE518A2CD7574DCB67060B005E24665EF532E0CCA73E1";
  const char str_out[] = "0XAF39E1F831CB4FCD92B17F61F473735C687593A931C97D2B60AD6C7443F09FDB";

  mpz_t N, out;
  mpz_init_set_str(N, str_N, 0);
  mpz_init(out);

  two_power_mod(512, N, out);

  // check output
  mpz_t out_ans;
  mpz_init_set_str(out_ans, str_out, 0);
  EXPECT_EQ(0, mpz_cmp(out,out_ans));
}

TEST(RsaTest, test_montgomery_to_self) {
  // MA(A, 2 ** 256) = A
  const char str_N[] = "0xE07122F2A4A9E81141ADE518A2CD7574DCB67060B005E24665EF532E0CCA73E1";
  const char str_A[] = "0x412820616369726641206874756F53202C48544542415A494C452054524F50";

  mpz_t A, B, N, out;
  mpz_init_set_str(A, str_A, 0);
  mpz_init_set_str(N, str_N, 0);
  mpz_inits(B, out, NULL);

  two_power_mod(256, N, B);
  montgomery_base2(A, B, N, out);

  // check output
  EXPECT_EQ(0, mpz_cmp(out, A));
}

TEST(RsaTest, test_montgomery) {
  // generate answer using RSA.py
  const char str_N[] = "0xE07122F2A4A9E81141ADE518A2CD7574DCB67060B005E24665EF532E0CCA73E1";
  const char str_A[] = "0x412820616369726641206874756f53202c48544542415a494c452054524f50";
  const char str_B[] = "0x10001";
  const char str_ans[] = "0x314f8acb18e57c4b2fa37adefa7964711b8dcdb7aac7514c78d97cf4d4121426";

  mpz_t A, B, N, out, ans;
  mpz_init_set_str(A, str_A, 0);
  mpz_init_set_str(B, str_B, 0);
  mpz_init_set_str(N, str_N, 0);
  mpz_init_set_str(ans, str_ans, 0);
  mpz_init(out);

  montgomery_base2(A, B, N, out);

  // check output
  EXPECT_EQ(0, mpz_cmp(out, ans));
}

TEST(RsaTest, test_lsb) {
  // generate answer using RSA.py
  const char str_N[] = "0xE07122F2A4A9E81141ADE518A2CD7574DCB67060B005E24665EF532E0CCA73E1";
  const char str_A[] = "0x412820616369726641206874756f53202c48544542415a494c452054524f50";
  const char str_B[] = "0x10001";
  const char str_ans[] = "0XC7F5B34038B0ED8E8B286C739B163BDDF7F4C60A7581A32F99C3F1FA99A474A5";

  mpz_t A, B, N, out, ans;
  mpz_init_set_str(A, str_A, 0);
  mpz_init_set_str(B, str_B, 0);
  mpz_init_set_str(N, str_N, 0);
  mpz_init_set_str(ans, str_ans, 0);
  mpz_init(out);

  lsb_modular_exponentiation(A, B, N, out);

  // check output
  EXPECT_EQ(0, mpz_cmp(out, ans));
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
