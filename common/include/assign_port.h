#pragma once

#include "verilated.h"
#include "verilog_int.h"

namespace verilog {

// assign port with privimite
void write_verilator_port(CData &dest, const uint64_t src) {
  dest = static_cast<CData>(src);
}
void write_verilator_port(SData &dest, const uint64_t src) {
  dest = static_cast<SData>(src);
}
void write_verilator_port(IData &dest, const uint64_t src) {
  dest = static_cast<IData>(src);
}
void write_verilator_port(QData &dest, const uint64_t src) {
  dest = static_cast<QData>(src);
}

// assign port with vuint
template <bool is_signed, unsigned num_bit>
void write_verilator_port(CData &dest,
                          const verilog::vint<is_signed, num_bit> src) {
  write_verilator_port(dest, src.v[0]);
}
template <bool is_signed, unsigned num_bit>
void write_verilator_port(SData &dest,
                          const verilog::vint<is_signed, num_bit> src) {
  write_verilator_port(dest, src.v[0]);
}
template <bool is_signed, unsigned num_bit>
void write_verilator_port(IData &dest,
                          const verilog::vint<is_signed, num_bit> src) {
  write_verilator_port(dest, src.v[0]);
}
template <bool is_signed, unsigned num_bit>
void write_verilator_port(QData &dest,
                          const verilog::vint<is_signed, num_bit> src) {
  write_verilator_port(dest, src.v[0]);
}

template <std::size_t Vlword>
void write_verilator_port(VlWide<Vlword> &dest, const uint64_t src) {
  static_assert(Vlword > 0);
  if constexpr (Vlword == 1) {
    write_verilator_port(dest.m_storage[0], src);
  } else {
    write_verilator_port(dest.m_storage[0], src & 0xFFFFFFFF);
    write_verilator_port(dest.m_storage[1], (src >> 32) & 0xFFFFFFFF);
    for (int i = 2; i < Vlword; i++) {
      write_verilator_port(dest.m_storage[i], 0);
    }
  }
}

template <std::size_t Vlword, unsigned num_bit>
void write_verilator_port(VlWide<Vlword> &dest,
                          const verilog::vuint<num_bit> &src) {
  static_assert(Vlword > 0);
  if constexpr (Vlword == 1) {
    write_verilator_port(dest.m_storage[0], src);
  } else if (src.num_word == 1) {
    write_verilator_port(dest, src.v[0]);
  } else {
    size_t num_u32 =
        std::min(Vlword, static_cast<std::size_t>(src.num_word * 2));
    for (size_t i = 0; i < num_u32; i++) {
      if (i % 2 == 0) {
        dest.m_storage[i] = src.v[i / 2] & 0xFFFFFFFF;
      } else {
        dest.m_storage[i] = (src.v[i / 2] >> 32) & 0xFFFFFFFF;
      }
    }
    for (size_t i = num_u32; i < Vlword; i++) {
      dest.m_storage[i] = 0;
    }
  }
}
} // namespace verilog