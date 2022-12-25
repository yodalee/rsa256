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

// read port to primitive
void read_verilator_port(uint64_t &dest, const CData &src) { dest = src; }
void read_verilator_port(uint64_t &dest, const SData &src) { dest = src; }
void read_verilator_port(uint64_t &dest, const IData &src) { dest = src; }
void read_verilator_port(uint64_t &dest, const QData &src) { dest = src; }

template <std::size_t Vlword>
void read_verilator_port(uint64_t &dest, const VlWide<Vlword> &src) {
  if constexpr (Vlword == 1) {
    read_verilator_port(dest, src.m_storage[0]);
  } else {
    dest = src.m_storage[0] | src.m_storage[1] << 32;
  }
}

template <unsigned num_bit>
void read_verilator_port(vuint<num_bit> &dest, CData &src) {
  dest = src;
}

template <unsigned num_bit>
void read_verilator_port(vuint<num_bit> &dest, SData &src) {
  dest = src;
}

template <unsigned num_bit>
void read_verilator_port(vuint<num_bit> &dest, IData &src) {
  dest = src;
}

template <unsigned num_bit>
void read_verilator_port(vuint<num_bit> &dest, QData &src) {
  dest = src;
}

template <unsigned num_bit, std::size_t Vlword>
void read_verilator_port(verilog::vuint<num_bit> &dest, VlWide<Vlword> &src) {
  constexpr size_t num_32b = (num_bit + 31) / 32;
  size_t src_word = 0;
  size_t dest_word = 0;

  for (dest_word = 0; dest_word < num_32b and src_word < Vlword;
       ++src_word, ++dest_word) {
    const uint64_t val = src[src_word];
    const size_t put_pos = dest_word / 2;

    if (dest_word % 2 == 0) {
      dest.v[put_pos] &= 0xffffffff00000000;
      dest.v[put_pos] |= val;
    } else {
      dest.v[put_pos] &= 0x00000000ffffffff;
      dest.v[put_pos] |= val << 32;
    }
  }
  // clear data longer than VlWide range
  for (; dest_word < num_32b; ++dest_word) {
    const size_t put_pos = dest_word / 2;
    if (dest_word % 2 == 0) {
      dest.v[put_pos] &= 0xffffffff00000000;
    } else {
      dest.v[put_pos] &= 0x00000000ffffffff;
    }
  }
  dest.ClearUnusedBits();
}

} // namespace verilog
