#include "verilog/dtype/vint.h"
#include "rsa.h"
#include <gtest/gtest.h>
#include <cassert>
#include <string>
using namespace std;

TEST(RsaTest, test_two_power_mod) {
  // using python to test generatation
  // gold = out = 2 ** 512 % in1, these are their hex values
  string str_in1 ("E07122F2A4A9E81141ADE518A2CD7574DCB67060B005E24665EF532E0CCA73E1");
  string str_gold("AF39E1F831CB4FCD92B17F61F473735C687593A931C97D2B60AD6C7443F09FDB");
  rsa_key_t in1, out, gold;
  from_hex(in1, str_in1);
  from_hex(gold, str_gold);

  // run and check
  two_power_mod(out, 512, in1);
  EXPECT_EQ(out, gold);
}

TEST(RsaTest, test_montgomery_to_self) {
  // MA(A, 2 ** 256) = A
  string str_N("E07122F2A4A9E81141ADE518A2CD7574DCB67060B005E24665EF532E0CCA73E1");
  string str_A("412820616369726641206874756F53202C48544542415A494C452054524F50");
  rsa_key_t A, B, N, out;
  from_hex(A, str_A);
  from_hex(N, str_N);

  // run and check
  two_power_mod(B, 256, N);
  montgomery_base2(out, A, B, N);
  EXPECT_EQ(out, A);
}

TEST(RsaTest, test_montgomery) {
  // generate answer using RSA.py
  string str_N("E07122F2A4A9E81141ADE518A2CD7574DCB67060B005E24665EF532E0CCA73E1");
  string str_A("412820616369726641206874756f53202c48544542415a494c452054524f50");
  string str_B("10001");
  string str_ans("314f8acb18e57c4b2fa37adefa7964711b8dcdb7aac7514c78d97cf4d4121426");
  rsa_key_t A, B, N, out, ans;
  from_hex(A, str_A);
  from_hex(B, str_B);
  from_hex(N, str_N);
  from_hex(ans, str_ans);
  // run and check
  montgomery_base2(out, A, B, N);
  EXPECT_EQ(out, ans);

  string str_sq("bf5ecf932790b9c808118ad16e41045d881920f60b031743c84d51b9246778a4");
  string str_ans2("6356aa97d0e04b2c4c9147e036a9439501f9a2c3e17c086be640372186ed94ae");
  from_hex(A, str_sq);
  from_hex(ans, str_ans2);
  // run and check
  montgomery_base2(out, A, A, N);
  EXPECT_EQ(out, ans);
}

TEST(RsaTest, test_lsb) {
  // generate answer using RSA.py
  string str_N("E07122F2A4A9E81141ADE518A2CD7574DCB67060B005E24665EF532E0CCA73E1");
  string str_A("412820616369726641206874756f53202c48544542415a494c452054524f50");
  string str_B("10001");
  string str_ans("C7F5B34038B0ED8E8B286C739B163BDDF7F4C60A7581A32F99C3F1FA99A474A5");
  rsa_key_t A, B, N, out, ans;
  from_hex(A, str_A);
  from_hex(B, str_B);
  from_hex(N, str_N);
  from_hex(ans, str_ans);

  // run and check
  lsb_modular_exponentiation(out, A, B, N);
  EXPECT_EQ(out, ans);
}

TEST(RsaTest, test_rsa) {
  // example from tech document
  // or using python hex(m0 ** key % N)
  string str_N("E07122F2A4A9E81141ADE518A2CD7574DCB67060B005E24665EF532E0CCA73E1");
  string str_key("10001");
  string str_m0("412820616369726641206874756F53202C48544542415A494C452054524F50");
  string str_c0("D41B183313D306ADCA09126F3FED6CDEC7DCDCE49DB5C85CB2A37F08C0F2E31");
  rsa_key_t N, key, m0, c0, out;
  from_hex(N, str_N);
  from_hex(key, str_key);
  from_hex(m0, str_m0);
  from_hex(c0, str_c0);

  // check output
  rsa(out, m0, key, N);
  EXPECT_EQ(out, c0);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
