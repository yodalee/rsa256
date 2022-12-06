#include "verilog_int.h"
#include <iostream>
#include <systemc>

constexpr int kBW = 256;
using KeyType = verilog::vuint<kBW>;
struct RSATwoPowerModIn {
  friend ::std::ostream &operator<<(::std::ostream &os,
                                    const RSATwoPowerModIn &v) {
    os << "{" << v.power << ", " << v.modulus << "}" << std::endl;
    return os;
  }

  verilog::vuint<32> power;
  KeyType modulus;
};
using RSATwoPowerModOut = KeyType;

struct RSAMontgomeryModIn {
  friend ::std::ostream &operator<<(::std::ostream &os,
                                    const RSAMontgomeryModIn &in) {
    os << "{" << in.a << ", " << in.b << ", " << in.modulus << "}" << std::endl;
    return os;
  }

  KeyType a;
  KeyType b;
  KeyType modulus;
};
using RSAMontgomeryModOut = KeyType;