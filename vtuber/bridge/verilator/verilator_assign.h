#pragma once

#include "verilated.h"
#include "verilog/dtype/vint.h"
#include "verilog/operation/bits.h"
#include "verilog/operation/unpack.h"
#include "verilog/operation/pack.h"

namespace verilog {
namespace verilator {

constexpr size_t to_verilator_num_word(unsigned num_bit) { return (num_bit + 31) / 32 ; }
template<unsigned num_bit> using verilator_dtype =
  typename detail::dtype_dict<false, detail::num_bit2dict_key(num_bit)>::dtype;
template<unsigned num_bit> using verilator_wdtype = VlWide<to_verilator_num_word(num_bit)>;

// write vuint to verilator port
template <unsigned num_bit>
void write_port_scalar(verilator_dtype<num_bit> &dst, const vint<false, num_bit> &src) {
  dst = src.value();
}

template <unsigned num_bit>
void write_port_scalar(verilator_wdtype<num_bit> &dst, const vint<false, num_bit> &src) {
  const size_t verilator_num_word = to_verilator_num_word(num_bit);
  for (size_t i = 0; i < (verilator_num_word/2); ++i) {
    dst.m_storage[i*2  ] = WData(src.v[i]      );
    dst.m_storage[i*2+1] = WData(src.v[i] >> 32);
  }
  if constexpr ((verilator_num_word%2) == 1) {
    dst.m_storage[verilator_num_word-1] = WData(src.v[verilator_num_word/2]);
  }
}

template <typename Dst, typename Src>
void write_port(Dst& dst, const Src& src) {
  static_assert(is_dtype_v<Src> || std::is_integral_v<Src>, 
    "Only works for Src is a verilog type or integral type");
  if constexpr (is_dtype_v<Src>) {
    auto tmp = pack(src);
    write_port_scalar(dst, tmp);
  } else {
    dst = src;
  }
}

// read vuint from verilator port
template <unsigned num_bit>
void read_port_scalar(vint<false, num_bit> &dst, const verilator_dtype<num_bit> &src) {
  dst = src;
}

template <unsigned num_bit>
void read_port_scalar(vint<false, num_bit> &dst, const verilator_wdtype<num_bit> &src) {
  const size_t verilator_num_word = to_verilator_num_word(num_bit);
  for (size_t i = 0; i < (verilator_num_word/2); ++i) {
    dst.v[i] = (
        (uint64_t(src.m_storage[i*2+1]) << 32)
      | (uint64_t(src.m_storage[i*2  ])      )
    );
  }
  if constexpr ((verilator_num_word%2) == 1) {
    dst.v[verilator_num_word/2] = uint64_t(src.m_storage[verilator_num_word-1]);
  }
}

template <typename Dst, typename Src>
void read_port(Dst& dst, const Src& src) {
  static_assert(is_dtype_v<Dst> || std::is_integral_v<Dst>,
    "Only works for Dst is a verilog type or integral type");
  if constexpr (is_dtype_v<Dst>) {
    if constexpr (is_vint_v<Dst>) {
      read_port_scalar(dst, src);
    } else {
      vint<false, verilog::bits_v<Dst>> tmp;
      read_port_scalar(tmp, src);
      unpack(dst, tmp);
    }
  } else {
    dst = src;
  }
}

} // namespace verilator
} // namespace verilog
