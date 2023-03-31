#pragma once

#include "verilog/dtype/vint.h"
#include "verilog/dtype/vstruct.h"
#include <iostream>
#include <systemc>

constexpr int kBW = 256;
using KeyType = verilog::vuint<kBW>;

struct RSATwoPowerModIn {
  using IntType = verilog::vuint<32>;
  IntType power;
  KeyType modulus;
  MAKE_VSTRUCT(power, modulus)
  VSTRUCT_HAS_PROCESS(RSATwoPowerModIn)
};
using RSATwoPowerModOut = KeyType;

struct RSAMontgomeryModIn {
  KeyType a;
  KeyType b;
  KeyType modulus;
  MAKE_VSTRUCT(a, b, modulus)
  VSTRUCT_HAS_PROCESS(RSAMontgomeryModIn)
};
using RSAMontgomeryModOut = KeyType;

struct RSAMontModIn {
  KeyType base; // 2 ^ 2n mod N
  KeyType msg;
  KeyType key;
  KeyType modulus;
  MAKE_VSTRUCT(base, msg, key, modulus)
  VSTRUCT_HAS_PROCESS(RSAMontModIn)
};
using RSAMontModOut = KeyType;

struct RSAModIn {
  KeyType msg;
  KeyType key;
  KeyType modulus;
  MAKE_VSTRUCT(msg, key, modulus)
  VSTRUCT_HAS_PROCESS(RSAModIn)
};
using RSAModOut = KeyType;