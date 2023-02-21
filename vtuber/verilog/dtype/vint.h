#pragma once
// Direct include
// C system headers
// C++ standard library headers
#include <cstdint>
#include <algorithm>
#include <cassert>
#include <iomanip>
#include <ostream>
#include <string>
#include <type_traits>
// Other libraries' .h files.
#include <immintrin.h>
// Your project's .h files.
#include "verilog/dtype_base.h"

namespace verilog {

namespace detail {

template<bool is_signed, unsigned num_bit> struct dtype_dict;
template<> struct dtype_dict<true, 0u> { typedef int8_t dtype; };
template<> struct dtype_dict<true, 1u> { typedef int16_t dtype; };
template<> struct dtype_dict<true, 2u> { typedef int32_t dtype; };
template<> struct dtype_dict<true, 3u> { typedef int64_t dtype; };
template<> struct dtype_dict<false, 0u> { typedef uint8_t dtype; };
template<> struct dtype_dict<false, 1u> { typedef uint16_t dtype; };
template<> struct dtype_dict<false, 2u> { typedef uint32_t dtype; };
template<> struct dtype_dict<false, 3u> { typedef uint64_t dtype; };

// this calculate the type to hold a num_bit integer
constexpr unsigned num_bit2dict_key(unsigned num_bit) {
	return (
		unsigned(num_bit > 8) +
		unsigned(num_bit > 16) +
		unsigned(num_bit > 32)
	);
}

constexpr unsigned num_bit2num_word(unsigned num_bit) {
	return (num_bit+63) / 64;
}

// These functions prevent implementation-defined specific behaviors in C standard
// Or make abstract if not easy to prevent

// _u_nsigned _s_hift _a_rith _r_ight
inline uint8_t usar(uint8_t a, unsigned b) { return uint8_t(int8_t(a) >> b); }
inline uint16_t usar(uint16_t a, unsigned b) { return uint16_t(int16_t(a) >> b); }
inline uint32_t usar(uint32_t a, unsigned b) { return uint32_t(int32_t(a) >> b); }
inline uint64_t usar(uint64_t a, unsigned b) { return uint64_t(int64_t(a) >> b); }

// to_signed
inline int8_t to_signed(uint8_t a) { return int8_t(a); }
inline int16_t to_signed(uint16_t a) { return int16_t(a); }
inline int32_t to_signed(uint32_t a) { return int32_t(a); }
inline int64_t to_signed(uint64_t a) { return int64_t(a); }

// shift n bits from hi to lo (two uint64_t)
// return lo
// 0 < n < 64
inline uint64_t shiftright128(uint64_t hi, uint64_t lo, unsigned n) {
	return (hi << (64-n)) | (lo >> n);
}

// shift n bits from lo to hi (two uint64_t)
// return hi
// 0 < n < 64
inline uint64_t shiftleft128(uint64_t hi, uint64_t lo, unsigned n) {
	return (hi << n) | (lo >> (64-n));
}

inline unsigned char addcarry64(uint64_t &out, uint64_t x, uint64_t y, unsigned char carry_in) {
	// Note: Impelemtnation-defined, modify me when necessary
	return _addcarry_u64(carry_in, x, y, reinterpret_cast<unsigned long long*>(&out));
}

inline unsigned char subborrow64(uint64_t &out, uint64_t x, uint64_t y, unsigned char carry_in) {
	// Note: Impelemtnation-defined, modify me when necessary
	return _subborrow_u64(carry_in, x, y, reinterpret_cast<unsigned long long*>(&out));
}

} // namespace detail

template <bool is_signed_, unsigned num_bit_>
struct vint {
	// Make template arguments accessible from outside
	static constexpr unsigned num_bit = num_bit_;
	static constexpr bool is_signed = is_signed_;
	static_assert(num_bit > 0);
	// Make vint compatible to dtype
	TAG_AS_VINT

	// cast to native C++ _d_ata type
	typedef typename detail::dtype_dict<is_signed, detail::num_bit2dict_key(num_bit)>::dtype dtype;
	// internal _s_torage type (we always use unsigned to store)
	typedef typename detail::dtype_dict<false, detail::num_bit2dict_key(num_bit)>::dtype stype;

	// bit width of stype and dtype
	static constexpr unsigned bw_word = 8 * sizeof(dtype);
	// how many stype required to store the vint
	static constexpr unsigned num_word = detail::num_bit2num_word(num_bit);
	// part of the most significant word is (un)used
	static constexpr unsigned unused_bit = num_word * bw_word - num_bit;
	static constexpr unsigned used_bit = bw_word - unused_bit;
	// The 1s of the used bits
	static constexpr stype used_mask = stype(-1) >> unused_bit;
	// The 1s of the unused bits
	static constexpr stype unused_mask = ~used_mask;
	// The 1 of the msb-bit
	static constexpr stype msb_mask = stype(1) << (used_bit - 1u);
	// vint holds 8, 16, 32, 64 bit data types that matches native C++ type
	static constexpr bool matched = num_bit == bw_word;
	// there might not be any totally unused word
	static_assert(unused_bit != bw_word);

	// the only data storage of vint
	// the ununsed bits must be kept zero (unsigned) or sign extended (signed)
	stype v[num_word];

	// rule-of-five related
	explicit vint(const stype rhs, bool sign_mode=is_signed) {
		assign(rhs, sign_mode);
	}

	vint& operator=(stype rhs) {
		assign(rhs, is_signed);
		return *this;
	}

	vint& assign(stype rhs, const bool sign_mode) {
		v[0] = rhs;
		if constexpr (num_word > 1) {
			stype extension_value = (
				(sign_mode and bool(rhs >> (bw_word-1u))) ?
				stype(-1) :
				stype(0)
			);
			::std::fill_n(::std::begin(v)+1, num_word-1, extension_value);
		}
		ClearUnusedBits();
		return *this;
	}

	vint() = default;
	vint(const vint&) = default;
	vint(vint&&) = default;
	vint& operator=(const vint &) = default;
	vint& operator=(vint &&) = default;

	stype SafeForOperation(stype x) const {
		x &= used_mask;
		return x;
	}

	void ClearUnusedBits() {
		v[num_word-1] = SafeForOperation(v[num_word-1]);
	}

	stype GetDtypeAtBitPos(unsigned pos) const {
		// This formula also for works num_bit < 64
		return v[pos/64u] >> (pos%64u);
	}

	// This function is designed to be used in from_hex/oct/hex exclusively
	// Use this at your risk
	void PutStypeAtBitPosUnsafe(unsigned pos, stype to_put) {
		// This formula also for works num_bit < 64
		v[pos/64u] |= to_put << (pos%64u);
	}

	//////////////////////
	// comparison
	//////////////////////

	// compare() will return:
	// 1: larger
	// 0: must check further
	// -1: smaller
	int compare_msb(stype rhs, bool sign_mode) const {
		stype lhs = v[num_word-1];
		if (sign_mode) {
			// flip the sign bit
			lhs ^= msb_mask;
			rhs ^= msb_mask;
		}
		if (lhs > rhs) {
			return 1;
		} else if (lhs < rhs) {
			return -1;
		}
		return 0;
	}

	int compare(const vint& rhs, const bool sign_mode) const {
		int cmp = compare_msb(rhs.v[num_word-1], sign_mode);
		if (cmp != 0) {
			return cmp;
		}
		if constexpr (num_word > 1) {
			for (unsigned i = num_word-1; i > 0;) {
				--i;
				if (v[i] > rhs.v[i]) {
					return 1;
				} else if (v[i] < rhs.v[i]) {
					return -1;
				}
			}
		}
		return 0;
	}

	int compare(const stype rhs, const bool sign_mode) const {
		if constexpr (num_word > 1) {
			const stype expected_sign = (sign_mode and to_signed(rhs) < 0) ? stype(-1) : stype(0);
			int cmp = compare_msb(SafeForOperation(expected_sign), sign_mode);
			if (cmp != 0) {
				return cmp;
			}
			for (unsigned i = num_word-1; i > 1;) {
				--i;
				if (v[i] > expected_sign) {
					return -1;
				} else if (v[i] < expected_sign) {
					return 1;
				}
			}
		} else {
			return compare_msb(SafeForOperation(rhs), sign_mode);
		}
		return 0;
	}

	int ucompare(const vint& rhs) const { return compare(rhs, false); }
	int ucompare(const stype rhs) const { return compare(rhs, false); }
	int scompare(const vint& rhs) const { return compare(rhs, true); }
	int scompare(const stype rhs) const { return compare(rhs, true); }

	bool operator==(const vint& rhs) const { return compare(rhs, is_signed) == 0; }
	bool operator>(const vint& rhs) const { return compare(rhs, is_signed) > 0; }
	bool operator<(const vint& rhs) const { return compare(rhs, is_signed) < 0; }
	bool operator>=(const vint& rhs) const { return compare(rhs, is_signed) >= 0; }
	bool operator<=(const vint& rhs) const { return compare(rhs, is_signed) <= 0; }
	bool operator==(const stype rhs) const { return compare(rhs, is_signed) == 0; }
	bool operator>(const stype rhs) const { return compare(rhs, is_signed) > 0; }
	bool operator<(const stype rhs) const { return compare(rhs, is_signed) < 0; }
	bool operator>=(const stype rhs) const { return compare(rhs, is_signed) >= 0; }
	bool operator<=(const stype rhs) const { return compare(rhs, is_signed) <= 0; }

	//////////////////////
	// add/sub/mul/div assignment
	//////////////////////
	vint& operator+=(const vint& rhs) {
		if constexpr (num_word == 1) {
			v[0] += rhs.v[0];
		} else {
			unsigned char carry = 0;
			for (unsigned i = 0; i < num_word; ++i) {
				carry = detail::addcarry64(v[i], v[i], rhs.v[i], carry);
			}
		}
		ClearUnusedBits();
		return *this;
	}

	vint& operator-=(const vint& rhs) {
		if constexpr (num_word == 1) {
			v[0] -= rhs.v[0];
		} else {
			unsigned char carry = 0;
			for (unsigned i = 0; i < num_word; ++i) {
				carry = detail::subborrow64(v[i], v[i], rhs.v[i], carry);
			}
		}
		ClearUnusedBits();
		return *this;
	}

	vint& operator*=(const vint& rhs) {
		static_assert(num_word == 1, "Multiplication > 64b is not supported");
		// 1u to force integer promotion to unsigned type
		v[0] = 1u * v[0] * rhs.v[0];
		ClearUnusedBits();
		return *this;
	}

	vint& operator+=(const stype rhs) {
		if constexpr (num_word == 1) {
			v[0] += rhs;
		} else {
			unsigned char carry = 0;
			carry = detail::addcarry64(v[0], v[0], rhs, carry);
			for (unsigned i = 1; i < num_word; ++i) {
				carry = detail::addcarry64(v[i], v[i], 0, carry);
			}
		}
		ClearUnusedBits();
		return *this;
	}

	vint& operator-=(const stype rhs) {
		if constexpr (num_word == 1) {
			v[0] -= rhs;
		} else {
			unsigned char carry = 0;
			carry = detail::subborrow64(v[0], v[0], rhs, carry);
			for (unsigned i = 1; i < num_word; ++i) {
				carry = detail::subborrow64(v[i], v[i], 0, carry);
			}
		}
		ClearUnusedBits();
		return *this;
	}

	vint& operator*=(const stype rhs) {
		static_assert(num_word == 1, "Multiplication > 64b is not supported");
		v[0] = v[0] * rhs;
		ClearUnusedBits();
		return *this;
	}

	//////////////////////
	// bitwise assignment
	//////////////////////
	vint& operator&=(const vint& rhs) {
		for (unsigned i = 0; i < num_word; ++i) {
			v[i] &= rhs[i];
		}
		return *this;
	}

	vint& operator|=(const vint& rhs) {
		for (unsigned i = 0; i < num_word; ++i) {
			v[i] |= rhs[i];
		}
		return *this;
	}

	vint& operator^=(const vint& rhs) {
		for (unsigned i = 0; i < num_word; ++i) {
			v[i] ^= rhs[i];
		}
		return *this;
	}

	vint& operator&=(const stype rhs) {
		v[0] &= rhs;
		ClearUnusedBits();
		return *this;
	}

	vint& operator|=(const stype rhs) {
		v[0] |= rhs;
		ClearUnusedBits();
		return *this;
	}

	vint& operator^=(const stype rhs) {
		v[0] ^= rhs;
		ClearUnusedBits();
		return *this;
	}

	//////////////////////
	// unary
	//////////////////////
	// TODO: and/or/xor reduction
	vint& Flip() {
		for (unsigned i = 0; i < num_word; ++i) {
			v[i] = ~v[i];
		}
		ClearUnusedBits();
		return *this;
	}

	vint operator~() const {
		vint ret = *this;
		return ret.Flip();
	}

	vint& Negate() {
		if constexpr (num_word == 1) {
			v[0] = -v[0];
		} else {
			unsigned char carry = 1;
			for (unsigned i = 0; i < num_word; ++i) {
				carry = detail::addcarry64(v[i], stype(~v[i]), stype(0), carry);
			}
		}
		ClearUnusedBits();
		return *this;
	}

	vint operator-() const {
		vint ret = *this;
		return ret.Negate();
	}

	explicit operator bool() {
		for (auto &x: v) {
			if (x) {
				return true;
			}
		}
		return false;
	}

	// explicit cast
	template<bool is_signed_dst, unsigned num_bit_dst>
	explicit operator vint<is_signed_dst, num_bit_dst>() const {
		vint<is_signed_dst, num_bit_dst> dst;
		typedef decltype(dst) dst_t;
		const bool is_neg = Bit(num_bit-1u);
		auto src_beg = ::std::begin(v);
		auto dst_beg = ::std::begin(dst.v);
		constexpr size_t num_word_min = ::std::min(dst_t::num_word, num_word);
		::std::copy_n(src_beg, num_word_min, dst_beg);
		if constexpr (num_bit_dst > num_bit) {
			constexpr unsigned bit_shifted =  num_bit_dst - num_bit;
			if (is_neg and dst_t::is_signed and is_signed) {
				dst.signext_after_shiftr(bit_shifted);
			} else {
				dst.clear_after_shiftr(bit_shifted);
			}
		} else if constexpr (num_bit_dst < num_bit) {
			dst.ClearUnusedBits();
		}
		return dst;
	}

	//////////////////////
	// shift assignment
	//////////////////////
	void signext_after_shiftr(const unsigned rhs) {
		const unsigned num_remaining = num_bit - rhs;
		const unsigned signext_from_word = detail::num_bit2num_word(num_remaining);
		const unsigned used_bit_after_shift_minus_one = (num_remaining-1u) % bw_word;
		if constexpr (num_word > 1) {
			assert(signext_from_word > 0); // since rhs != 0, this shall not fail
			::std::fill(v+signext_from_word, v+num_word, stype(-1));
		}
		v[signext_from_word-1] |= stype(-2) << used_bit_after_shift_minus_one;
		ClearUnusedBits();
	}

	void clear_after_shiftr(const unsigned rhs) {
		if constexpr (num_word > 1) {
			const unsigned num_remaining = num_bit - rhs;
			const unsigned signext_from_word = detail::num_bit2num_word(num_remaining);
			const unsigned unused_bit_after_shift = (-num_remaining) % bw_word;
			assert(signext_from_word > 0); // since rhs != 0, this shall not fail
			::std::fill(v+signext_from_word, v+num_word, stype(0));
			v[signext_from_word-1] &= stype(-1) >> unused_bit_after_shift;
		}
	}

	// >>=
	vint& shiftopr(const unsigned rhs, const bool sign_mode) {
		// rhs >= num_bit is UB in C, but not sure in Verilog
		if (rhs == 0 or rhs >= num_bit) {
			// do nothing
			return *this;
		}
		const bool do_sign_extension = sign_mode and Bit(num_bit-1u);
		if constexpr (num_word == 1) {
			v[0] >>= rhs;
		} else {
			const unsigned word_shift = rhs / 64;
			const unsigned bit_shift = rhs % 64;
			if (bit_shift == 0) {
				for (unsigned i = word_shift; i < num_word; ++i) {
					v[i-word_shift] = v[i];
				}
			} else {
				for (unsigned i = word_shift; i < num_word-1; ++i) {
					v[i-word_shift] = detail::shiftright128(v[i+1], v[i], bit_shift);
				}
				v[num_word-word_shift-1] = v[num_word-1] >> bit_shift;
			}
		}
		if (do_sign_extension) {
			signext_after_shiftr(rhs);
		} else {
			clear_after_shiftr(rhs);
		}
		return *this;
	}

	vint& operator>>=(const unsigned rhs) {
		return shiftopr(rhs, is_signed);
	}

	vint& operator<<=(const unsigned rhs) {
		// rhs >= num_bit is UB in C, but not sure in Verilog
		if (rhs == 0 or rhs >= num_bit) {
			// do nothing
			return *this;
		}

		if constexpr (num_word == 1) {
			v[0] <<= rhs;
		} else {
			unsigned word_shift = rhs / 64u;
			unsigned bit_shift = rhs % 64u;
			if (bit_shift == 0) {
				for (unsigned i = num_word; i > word_shift;) {
					--i;
					v[i] = v[i-word_shift];
				}
			} else {
				for (unsigned i = num_word; i > word_shift+1;) {
					--i;
					v[i] = detail::shiftleft128(v[i-word_shift], v[i-word_shift-1], bit_shift);
				}
				v[word_shift] = v[0] << bit_shift;
			}
			::std::fill_n(::std::begin(v), word_shift, 0);
		}
		ClearUnusedBits();
		return *this;
	}

	template <bool is_signed2, unsigned num_bit2>
	vint& operator>>=(const vint<is_signed2, num_bit2>& rhs) {
		*this >>= rhs.uvalue();
	}

	template <bool is_signed2, unsigned num_bit2>
	vint& operator<<=(const vint<is_signed2, num_bit2>& rhs) {
		*this <<= rhs.uvalue();
	}

	//////////////////////
	// derived operators
	//////////////////////
	vint operator+(const vint& rhs) { vint ret = *this; ret += rhs; return ret; }
	vint operator-(const vint& rhs) { vint ret = *this; ret -= rhs; return ret; }
	vint operator*(const vint& rhs) { vint ret = *this; ret *= rhs; return ret; }
	vint operator/(const vint& rhs) { vint ret = *this; ret /= rhs; return ret; }
	vint operator&(const vint& rhs) { vint ret = *this; ret += rhs; return ret; }
	vint operator|(const vint& rhs) { vint ret = *this; ret -= rhs; return ret; }
	vint operator^(const vint& rhs) { vint ret = *this; ret ^= rhs; return ret; }
	vint operator>>(const vint& rhs) { vint ret = *this; ret >>= rhs; return ret; }
	vint operator<<(const vint& rhs) { vint ret = *this; ret <<= rhs; return ret; }
	bool operator!=(const vint& rhs) const { return not (*this == rhs); }

	vint operator+(const stype rhs) { vint ret = *this; ret += rhs; return ret; }
	vint operator-(const stype rhs) { vint ret = *this; ret -= rhs; return ret; }
	vint operator*(const stype rhs) { vint ret = *this; ret *= rhs; return ret; }
	vint operator/(const stype rhs) { vint ret = *this; ret /= rhs; return ret; }
	vint operator&(const stype rhs) { vint ret = *this; ret &= rhs; return ret; }
	vint operator|(const stype rhs) { vint ret = *this; ret |= rhs; return ret; }
	vint operator^(const stype rhs) { vint ret = *this; ret ^= rhs; return ret; }
	vint operator>>(const stype rhs) { vint ret = *this; ret >>= rhs; return ret; }
	vint operator<<(const stype rhs) { vint ret = *this; ret <<= rhs; return ret; }
	bool operator!=(const stype rhs) const { return not (*this == rhs); }

	//////////////////////
	// slice
	//////////////////////
	bool Bit(unsigned pos) const {
		assert(pos < num_bit);
		const unsigned shamt =  pos % bw_word;
		const unsigned lsb_word = pos / bw_word;
		return bool((v[lsb_word] >> shamt) & 1u);
	}

	void SetBit(unsigned pos, bool value) {
		assert(pos < num_bit);
		const unsigned shamt =  pos % bw_word;
		const unsigned lsb_word = pos / bw_word;
		const stype bmask = stype(1) << shamt;
		v[lsb_word] &= ~bmask;
		if (value) {
			v[lsb_word] |= bmask;
		}
	}

	template<bool is_signed>
	void SetBit(unsigned pos, vint<is_signed, 1u> value) {
		assert(pos < num_bit);
		SetBit(pos, bool(value));
	}

	template<unsigned pos, unsigned num_bit_slice>
	vint<false, num_bit_slice> Slice() const {
		static_assert(pos + num_bit_slice <= num_bit);
		vint<false, num_bit_slice> sl;
		constexpr unsigned shamt = pos % bw_word;
		constexpr unsigned lsb_word = pos / bw_word;
		constexpr unsigned num_word_slice = detail::num_bit2num_word(num_bit_slice);
		if constexpr (shamt == 0) {
			for (unsigned i = 0; i < num_word_slice; ++i) {
				sl.v[i] = v[i+lsb_word];
			}
		} else {
			for (unsigned i = 0; i < num_word_slice - 1u; ++i) {
				sl.v[i] = detail::shiftright128(v[i+lsb_word+1], v[i+lsb_word], shamt);
			}
			constexpr unsigned required_msb_word = num_word_slice+lsb_word;
			if constexpr (required_msb_word == num_word) {
				sl.v[num_word_slice-1] = v[num_word-1] >> shamt;
			} else {
				sl.v[num_word_slice-1] = detail::shiftright128(
					v[required_msb_word],
					v[required_msb_word-1],
					shamt
				);
			}
		}
		sl.ClearUnusedBits();
		return sl;
	}

	template<unsigned lsb_pos, unsigned num_bit_slice>
	void ClearSlice() {
		static_assert(lsb_pos + num_bit_slice <= num_bit);
		constexpr unsigned msb_pos = lsb_pos + num_bit_slice - 1;
		constexpr unsigned lsb_word_slice = lsb_pos / bw_word;
		constexpr unsigned msb_word_slice = msb_pos / bw_word;
		constexpr unsigned unused_lsb_slice = lsb_pos % bw_word;
		constexpr unsigned unused_msb_slice = bw_word - 1 - (msb_pos % bw_word);
		// create somethig like 0b1111000 and 0b00001111
		// TODO: any clear way to prevent gcc warning elegantly?
		constexpr stype lsb_mask_slice = (stype(-1) >> unused_lsb_slice << unused_lsb_slice) ^ stype(-1);
		constexpr stype msb_mask_slice = (stype(-1) >> unused_msb_slice) ^ stype(-1);
		if constexpr (lsb_word_slice == msb_word_slice) {
			v[lsb_word_slice] &= lsb_mask_slice | msb_mask_slice;
		} else {
			v[lsb_word_slice] &= lsb_mask_slice;
			for (unsigned i = lsb_word_slice+1; i < msb_word_slice; ++i) {
				v[i] = 0;
			}
			v[msb_word_slice] &= msb_mask_slice;
		}
	}

	template<unsigned lsb_pos, unsigned num_bit_slice>
	void WriteSliceUnsafe(const vint<false, num_bit_slice>& sl) {
		static_assert(lsb_pos + num_bit_slice <= num_bit);
		constexpr unsigned msb_pos = lsb_pos + num_bit_slice - 1;
		constexpr unsigned lsb_word_slice = lsb_pos / bw_word;
		constexpr unsigned msb_word_slice = msb_pos / bw_word;
		constexpr unsigned num_word_slice = detail::num_bit2num_word(num_bit_slice);
		constexpr unsigned unused_lsb_slice = lsb_pos % bw_word;
		if constexpr (unused_lsb_slice == 0) {
			for (unsigned i = 0; i < num_word_slice; ++i) {
				v[lsb_word_slice+i] |= sl.v[i];
			}
		} else {
			v[lsb_word_slice] |= stype(sl.v[0]) << unused_lsb_slice;
			for (unsigned i = 1; i < num_word_slice; ++i) {
				v[lsb_word_slice+i] |= detail::shiftleft128(sl.v[i], sl.v[i-1], unused_lsb_slice);
			}
			if constexpr (msb_word_slice == lsb_word_slice+num_word_slice) {
				v[msb_word_slice] |= stype(sl.v[num_word_slice-1]) >> (bw_word - unused_lsb_slice);
			}
		}
	}

	template<unsigned lsb_pos, unsigned num_bit_slice>
	void SetSlice(const vint<false, num_bit_slice>& sl) {
		ClearSlice<lsb_pos, num_bit_slice>();
		WriteSliceUnsafe<lsb_pos>(sl);
	}

	//////////////////////
	// others
	//////////////////////
	dtype value() const {
		if constexpr (is_signed) {
			return svalue();
		} else {
			return uvalue();
		}
	}

	stype uvalue() const {
		return v[0];
	}

	dtype svalue() const {
		stype ret = v[0];
		const bool is_neg = Bit(num_bit-1u);
		if constexpr (num_word == 1) {
			if (is_neg) {
				ret |= unused_mask;
			}
		}
		return detail::to_signed(ret);
	}

	[[gnu::noinline]]
	friend void from_hex(vint &val, const ::std::string &s) {
		constexpr unsigned max_len = (num_bit + 3) / 4;
		::std::fill_n(::std::begin(val.v), num_word, 0);
		if (s.empty()) {
			return;
		}
		unsigned str_pos = s.size()-1;
		unsigned put_pos = 0;
		int to_put;
		// pos == size_t(-1) is safe according to the standard
		for (put_pos = 0; put_pos < 4*max_len and str_pos != -1; --str_pos) {
			const char c = s[str_pos];
			if ('0' <= c and c <= '9') {
				to_put = c - '0';
			} else if ('a' <= c and c <= 'f') {
				to_put = c - 'a' + 10;
			} else if ('A' <= c and c <= 'F') {
				to_put = c - 'A' + 10;
			} else {
				continue;
			}
			val.PutStypeAtBitPosUnsafe(put_pos, to_put);
			put_pos += 4;
		}
		val.ClearUnusedBits();
		if (put_pos == 0) {
			return;
		}

		// Handle negative & sign extension for strings started with - or '
		if (to_put != 0 and s[0] == '\'') {
			const unsigned to_put_extended = (
				to_put >= 8 ?  to_put        : (
				to_put >= 4 ? (to_put | 0x8) : (
				to_put >= 2 ? (to_put | 0xc) :
				                        0xf
			)));
			// fill previously filled character
			val.PutStypeAtBitPosUnsafe(put_pos-4u, to_put_extended);
			// fill from current position to the end
			for (; put_pos < 4*max_len; put_pos += 4) {
				val.PutStypeAtBitPosUnsafe(put_pos, 0xf);
			}
			val.ClearUnusedBits();
		} else if (s[0] == '-') {
			val.Negate();
		}
	}

	[[gnu::noinline]]
	friend void from_bin(vint &val, const ::std::string &s) {
		constexpr unsigned max_len = num_bit;
		::std::fill_n(::std::begin(val.v), num_word, 0);
		if (s.empty()) {
			return;
		}
		unsigned str_pos = s.size()-1;
		unsigned put_pos = 0;
		int to_put;
		// pos == size_t(-1) is safe according to the standard
		for (put_pos = 0; put_pos < max_len and str_pos != -1; --str_pos) {
			const char c = s[str_pos];
			if ('0' <= c and c <= '1') {
				to_put = c - '0';
			} else {
				continue;
			}
			val.PutStypeAtBitPosUnsafe(put_pos, to_put);
			put_pos += 1;
		}
		if (put_pos == 0) {
			return;
		}

		// Handle negative & sign extension for strings started with - or '
		if (to_put != 0 and s[0] == '\'') {
			// fill from current position to the end
			for (; put_pos < max_len; put_pos += 1) {
				val.PutStypeAtBitPosUnsafe(put_pos, 0x1);
			}
			val.ClearUnusedBits();
		} else if (s[0] == '-') {
			val.Negate();
		}
	}

	[[gnu::noinline]]
	friend ::std::string to_hex(const vint &val) {
		::std::string ret;
		constexpr unsigned max_len = (num_bit+3) / 4;
		constexpr unsigned msb_hex_pos = 4 * (max_len-1);
		constexpr int num_bit_msb_hex = ((num_bit-1) % 4) + 1;
		constexpr int msb_mask = (1<<num_bit_msb_hex) - 1;
		ret.reserve(max_len);

		bool met_nonzero = false;
		for (unsigned i = 0; i < 4*max_len; i += 4) {
			const int cur_mask = i == 0 ? msb_mask : 0xf;
			const int cur_hex = val.GetDtypeAtBitPos(msb_hex_pos - i) & cur_mask;
			// Set met_nonzero if not yet and meet a nonzero
			// After met_nonzero, we start add characters.
			if (met_nonzero or cur_hex != 0) {
				met_nonzero = true;
			}
			if (met_nonzero) {
				ret.push_back(
					cur_hex >= 10 ?
					(cur_hex + 'A' - 10) :
					(cur_hex + '0')
				);
			}
		}
		if (!met_nonzero) {
			// the vint is zero
			ret.push_back('0');
		}
		return ret;
	}

	friend void from_string(vint &val, ::std::string s, unsigned base=16u) {
		switch (base) {
			case 2: {
				from_bin(val);
				break;
			}
			case 16: {
				from_hex(val);
				break;
			}
			default: {
				assert(0);
			}
		}
	}

	friend ::std::ostream& operator<<(::std::ostream& os, const vint &v) {
		if constexpr (num_word == 1) {
			os << +v.value();
		} else {
			os << to_hex(v);
		}
		return os;
	}

	explicit vint(::std::string s, unsigned base=16u) {
		from_string(*this, s, base);
	}

};

template<unsigned num_bit> using vsint = vint<true, num_bit>;
template<unsigned num_bit> using vuint = vint<false, num_bit>;

namespace detail {

template<unsigned cur_ofs, unsigned total_bits>
struct ConcatProxy {
	vint<false, total_bits> &target_;
	ConcatProxy(vint<false, total_bits> &target): target_(target) {}
};

template<unsigned cur_ofs, unsigned total_bits, unsigned add_num_bit>
auto operator+(
	const vint<false, add_num_bit>& rhs,
	ConcatProxy<cur_ofs, total_bits> proxy
) {
	proxy.target_.template WriteSliceUnsafe<cur_ofs>(rhs);
	return ConcatProxy<cur_ofs + add_num_bit, total_bits>(proxy.target_);
}

} // detail

template<unsigned...num_bits>
auto concat(const vint<false, num_bits>&...values) {
	constexpr unsigned total_bits = (num_bits + ...);
	vint<false, total_bits> ret;
	::std::fill(::std::begin(ret.v), ::std::end(ret.v), 0);
	(values + ... + detail::ConcatProxy<0u, total_bits>(ret));
	return ret;
}

} // namespace verilog
