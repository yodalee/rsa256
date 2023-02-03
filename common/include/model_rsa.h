#pragma once

#include "verilog/dtype/vint.h"
#include <iostream>
#include <systemc>

constexpr int kBW = 256;
using KeyType = verilog::vuint<kBW>;

struct RSAModIn {
  friend ::std::ostream &operator<<(::std::ostream &os, const RSAModIn &v) {
    os << typeid(v).name() << "{" << v.msg << ", " << v.key << ", " << v.modulus
       << ::std::endl;
    return os;
  }
  KeyType msg;
  KeyType key;
  KeyType modulus;
};
using RSAModOut = KeyType;

struct RSATwoPowerModIn {
  using TwoPowerMod_Power_t = verilog::vuint<32>;
  friend ::std::ostream &operator<<(::std::ostream &os,
                                    const RSATwoPowerModIn &v) {
    os << "{" << v.power << ", " << v.modulus << "}" << std::endl;
    return os;
  }

  TwoPowerMod_Power_t power;
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
