#include "rsa.h"
#include "verilog/dtype/vint.h"
#include <gtest/gtest.h>
#include <string>
using namespace std;

TEST(RsaTest, test_two_power_mod) {
  // using python to test generatation
  // gold = out = 2 ** 512 % in1, these are their hex values
  string str_in1(
      "E07122F2A4A9E81141ADE518A2CD7574DCB67060B005E24665EF532E0CCA73E1");
  string str_gold(
      "AF39E1F831CB4FCD92B17F61F473735C687593A931C97D2B60AD6C7443F09FDB");
  rsa_key_t in1, out, gold;
  from_hex(in1, str_in1);
  from_hex(gold, str_gold);

  // run and check
  twopower(out, 512, in1);
  EXPECT_EQ(out, gold);
}

TEST(RsaTest, test_montgomery2_to_self) {
  // MA(A, 2 ** 256) = A
  string str_N(
      "E07122F2A4A9E81141ADE518A2CD7574DCB67060B005E24665EF532E0CCA73E1");
  string str_A(
      "412820616369726641206874756F53202C48544542415A494C452054524F50");
  rsa_key_t A, B, N, out;
  from_hex(A, str_A);
  from_hex(N, str_N);

  // run and check
  twopower(B, 256, N);
  montgomery_base2(out, A, B, N);
  EXPECT_EQ(out, A);
}

TEST(RsaTest, test_montgomery4_to_self) {
  // MA(A, 2 ** 256) = A
  string str_N(
      "E07122F2A4A9E81141ADE518A2CD7574DCB67060B005E24665EF532E0CCA73E1");
  string str_A(
      "412820616369726641206874756F53202C48544542415A494C452054524F50");
  rsa_key_t A, B, N, out;
  from_hex(A, str_A);
  from_hex(N, str_N);

  // run and check
  twopower(B, 256, N);
  montgomery_base4(out, A, B, N);
  EXPECT_EQ(out, A);
}

TEST(RsaTest, test_montgomery2) {
  // generate answer using RSA.py
  string str_N(
      "E07122F2A4A9E81141ADE518A2CD7574DCB67060B005E24665EF532E0CCA73E1");
  string str_A(
      "412820616369726641206874756f53202c48544542415a494c452054524f50");
  string str_B("10001");
  string str_ans(
      "314f8acb18e57c4b2fa37adefa7964711b8dcdb7aac7514c78d97cf4d4121426");
  rsa_key_t A, B, N, out, ans;
  from_hex(A, str_A);
  from_hex(B, str_B);
  from_hex(N, str_N);
  from_hex(ans, str_ans);
  // run and check
  montgomery_base2(out, A, B, N);
  EXPECT_EQ(out, ans);

  string str_sq(
      "bf5ecf932790b9c808118ad16e41045d881920f60b031743c84d51b9246778a4");
  string str_ans2(
      "6356aa97d0e04b2c4c9147e036a9439501f9a2c3e17c086be640372186ed94ae");
  from_hex(A, str_sq);
  from_hex(ans, str_ans2);
  // run and check
  montgomery_base2(out, A, A, N);
  EXPECT_EQ(out, ans);
}

TEST(RsaTest, test_montgomery4) {
  // generate answer using RSA.py
  string str_N(
      "E07122F2A4A9E81141ADE518A2CD7574DCB67060B005E24665EF532E0CCA73E1");
  string str_A(
      "412820616369726641206874756f53202c48544542415a494c452054524f50");
  string str_B("10001");
  string str_ans(
      "314f8acb18e57c4b2fa37adefa7964711b8dcdb7aac7514c78d97cf4d4121426");
  rsa_key_t A, B, N, out, ans;
  from_hex(A, str_A);
  from_hex(B, str_B);
  from_hex(N, str_N);
  from_hex(ans, str_ans);
  // run and check
  montgomery_base4(out, A, B, N);
  EXPECT_EQ(out, ans);

  string str_sq(
      "bf5ecf932790b9c808118ad16e41045d881920f60b031743c84d51b9246778a4");
  string str_ans2(
      "6356aa97d0e04b2c4c9147e036a9439501f9a2c3e17c086be640372186ed94ae");
  from_hex(A, str_sq);
  from_hex(ans, str_ans2);
  // run and check
  montgomery_base2(out, A, A, N);
  EXPECT_EQ(out, ans);
}

TEST(RsaTest, test_lsb) {
  // generate answer using RSA.py
  string str_N(
      "E07122F2A4A9E81141ADE518A2CD7574DCB67060B005E24665EF532E0CCA73E1");
  string str_A(
      "412820616369726641206874756f53202c48544542415a494c452054524f50");
  string str_B("10001");
  string str_ans(
      "C7F5B34038B0ED8E8B286C739B163BDDF7F4C60A7581A32F99C3F1FA99A474A5");
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
  string str_N(
      "E07122F2A4A9E81141ADE518A2CD7574DCB67060B005E24665EF532E0CCA73E1");
  string str_key("10001");
  string str_m0(
      "412820616369726641206874756F53202C48544542415A494C452054524F50");
  string str_c0(
      "D41B183313D306ADCA09126F3FED6CDEC7DCDCE49DB5C85CB2A37F08C0F2E31");
  rsa_key_t N, key, m0, c0, out;
  from_hex(N, str_N);
  from_hex(key, str_key);
  from_hex(m0, str_m0);
  from_hex(c0, str_c0);

  // check output
  rsa(out, m0, key, N);
  EXPECT_EQ(out, c0);
}

struct RsaData {
  string N;
  string key;
  string m;
  string c;
};

class RsaParameterSuite : public ::testing::TestWithParam<RsaData> {};

TEST_P(RsaParameterSuite, RsaParameterTest) {
  auto param = GetParam();
  rsa_key_t N, key, m, c, out;
  from_hex(N, param.N);
  from_hex(key, param.key);
  from_hex(m, param.m);
  from_hex(c, param.c);

  // check output
  rsa(out, m, key, N);
  EXPECT_EQ(out, c);
}

const vector<RsaData> dclabdataset{
    RsaData{
        .N = "E07122F2A4A9E81141ADE518A2CD7574DCB67060B005E24665EF532E0CCA73E1",
        .key = "10001",
        // P)-Don't call the Dutch underac
        .m = "63617265646E7520686374754420656874206C6C61632074276E6F442D2950",
        .c = "2EE3A71DAA16525340F365965046E0D01E348E4619ABFDBC81EA4E4799039ADF",
    },
    RsaData{
        .N = "E07122F2A4A9E81141ADE518A2CD7574DCB67060B005E24665EF532E0CCA73E1",
        .key = "10001",
        // hievers anymore. Not after the
        .m = "2065687420726574666120746F4E202E65726F6D796E612073726576656968",
        .c = "4E7EC5ACEBEC5CE50CB46DF13D251FA652CB89E9157B1F20C062CD4ECD20D801",
    },
    RsaData{
        .N = "E07122F2A4A9E81141ADE518A2CD7574DCB67060B005E24665EF532E0CCA73E1",
        .key = "10001",
        // way the Netherlands rallied to
        .m = "206F74206465696C6C61722073646E616C72656874654E2065687420796177",
        .c =
            "5A45C7C2D977C762F525888C55A025E5F621E11BDFBF82F91B3D9E46EE2D3931"},
    RsaData{
        .N = "E07122F2A4A9E81141ADE518A2CD7574DCB67060B005E24665EF532E0CCA73E1",
        .key = "10001",
        // upset five-time champion Brazil
        .m = "6C697A617242206E6F69706D61686320656D69742D65766966207465737075",
        .c =
            "C60BFA4C927DDF1A4F1E55C3AD10E65858AC609E668C2092E17D90D6AF29B71A"},
    RsaData{
        .N = "E07122F2A4A9E81141ADE518A2CD7574DCB67060B005E24665EF532E0CCA73E1",
        .key = "10001",
        // 2-1 in the World Cup quarterfi
        .m = "6966726574726175712070754320646C726F5720656874206E6920312D3220",
        .c = "18C9E09D871B4B1EF5DD844C2BF9B849A70E785652786421E24939C8EDADFDEE",
    },
    RsaData{
        .N = "E07122F2A4A9E81141ADE518A2CD7574DCB67060B005E24665EF532E0CCA73E1",
        .key = "10001",
        // nals Friday. After waking thems
        .m = "736D65687420676E696B6177207265746641202E79616469724620736C616E",
        .c = "39D7C84C563C9A593A66291C72A0F7FBBEF7B3F21DFEDE437325229EEF07F6B6",
    },
};

INSTANTIATE_TEST_SUITE_P(DcLabSet, RsaParameterSuite,
                         ::testing::ValuesIn(dclabdataset));

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
