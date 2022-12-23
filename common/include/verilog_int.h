#pragma once
#include <cstdint>
#include <immintrin.h>
#include <algorithm>
#include <cassert>
#include <iomanip>
#include <ostream>
#include <string>

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

// Shift Right Logical 64-bit
// ^     ^     ^       ^^
inline int64_t srl64(int64_t x, unsigned n) {
	// Note: Impelemtnation-defined, modify me when necessary
	return int64_t(uint64_t(x)>>n);
}

inline uint64_t srl64(uint64_t x, unsigned n) {
	return x>>n;
}

// Shift Right Arithmetic 64-bit
// ^     ^     ^          ^^
inline int64_t sra64(int64_t x, unsigned n) {
	return x>>n;
}

inline uint64_t sra64(uint64_t x, unsigned n) {
	// Note: Impelemtnation-defined, modify me when necessary
	return uint64_t(int64_t(x)>>n);
}

// Put the n-bit LSB of l2h to MSB
// Put the (64-n)-bit MSB of h2l to LSB
// 0 < n < 64
template<typename T>
inline T interleave64(T h2l, T l2h, unsigned n) {
	return srl64(h2l, n) | (l2h << (64-n));
}

inline unsigned char addcarry64(uint64_t &out, uint64_t x, uint64_t y, unsigned char carry_in) {
	// Note: Impelemtnation-defined, modify me when necessary
	return _addcarry_u64(carry_in, x, y, reinterpret_cast<unsigned long long*>(&out));
}

inline unsigned char addcarry64(int64_t &out, int64_t x, int64_t y, unsigned char carry_in) {
	// Note: Impelemtnation-defined, modify me when necessary
	return _addcarry_u64(carry_in, x, y, reinterpret_cast<unsigned long long*>(&out));
}

inline unsigned char subborrow64(uint64_t &out, uint64_t x, uint64_t y, unsigned char carry_in) {
	// Note: Impelemtnation-defined, modify me when necessary
	return _subborrow_u64(carry_in, x, y, reinterpret_cast<unsigned long long*>(&out));
}

inline unsigned char subborrow64(int64_t &out, int64_t x, int64_t y, unsigned char carry_in) {
	// Note: Impelemtnation-defined, modify me when necessary
	return _subborrow_u64(carry_in, x, y, reinterpret_cast<unsigned long long*>(&out));
}

} // namespace detail

template <bool is_signed, unsigned num_bit_>
struct vint {
	// Make num_bit accessible from outside
	static constexpr unsigned num_bit = num_bit_;
	static_assert(num_bit > 0);
	typedef typename detail::dtype_dict<is_signed, detail::num_bit2dict_key(num_bit)>::dtype dtype;
	static constexpr unsigned bw_word = 8 * sizeof(dtype);
	static constexpr unsigned num_word = detail::num_bit2num_word(num_bit);
	static constexpr unsigned ununsed_bit = num_word * bw_word - num_bit;
	// dtype == vint<is_signed, num_bit>
	static constexpr bool matched = num_bit == bw_word;
	// the ununsed bits must be kept zero (unsigned) or sign extended (signed)
	dtype v[num_word];

	// rule-of-five related
	explicit vint(dtype rhs) {
		*this = rhs;
	}

	vint& operator=(dtype rhs) {
		v[0] = rhs;
		if constexpr (num_word > 1) {
			::std::fill_n(
				::std::begin(v)+1, num_word-1,
				((rhs < 0) ? -1 : 0)
			);
		} else {
			ClampBits();
		}
		return *this;
	}

	explicit vint(::std::string s, int base=16) {
#ifndef NDEBUG
		assert(base == 2 or base == 8 or base == 16);
#endif
	}

	vint() = default;
	vint(const vint&) = default;
	vint(vint&&) = default;
	vint& operator=(const vint &) = default;
	vint& operator=(vint &&) = default;

	dtype SafeForOperation(dtype x) const {
		if constexpr (!matched) {
			x <<= ununsed_bit;
			x >>= ununsed_bit;
		}
		return x;
	}

	void ClampBits() {
		v[num_word-1] = SafeForOperation(v[num_word-1]);
	}

	dtype GetDtypeAtBitPos(unsigned pos) const {
		// This formula also for works num_bit < 64
		return v[pos/64] >> (pos%64);
	}

	void PutDtypeAtBitPosClean(unsigned pos, dtype to_put) {
		// This formula also for works num_bit < 64
		v[pos/64] |= to_put << (pos%64);
	}

	//////////////////////
	// comparison
	//////////////////////

	// compare() will return:
	// 1: larger
	// 0: must check further
	// -1: smaller
	int compare(const vint& rhs) const {
		if constexpr (num_word > 1) {
			for (unsigned i = num_word; i > 0;) {
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

	int compare(const dtype rhs) const {
		if constexpr (num_word > 1) {
			const dtype expected_sign = (is_signed and rhs < 0) ? -1 : 0;
			for (unsigned i = num_word; i > 1;) {
				--i;
				if (v[i] > expected_sign) {
					return -1;
				} else if (v[i] < expected_sign) {
					return 1;
				}
			}
		}
		return 0;
	}

	bool operator==(const vint& rhs) const { return compare(rhs) == 0; }
	bool operator>(const vint& rhs) const { return compare(rhs) > 0; }
	bool operator<(const vint& rhs) const { return compare(rhs) < 0; }
	bool operator>=(const vint& rhs) const { return compare(rhs) >= 0; }
	bool operator<=(const vint& rhs) const { return compare(rhs) <= 0; }

	bool operator==(dtype rhs) const {
		rhs = SafeForOperation(rhs);
		return compare(rhs) == 0 and v[0] == rhs;
	}

	bool operator>(dtype rhs) const {
		rhs = SafeForOperation(rhs);
		const int cmp = compare(rhs);
		return cmp > 0 or cmp == 0 and v[0] > rhs;
	}

	bool operator<(dtype rhs) const {
		rhs = SafeForOperation(rhs);
		const int cmp = compare(rhs);
		return cmp < 0 or cmp == 0 and v[0] < rhs;
	}

	bool operator>=(dtype rhs) const {
		rhs = SafeForOperation(rhs);
		const int cmp = compare(rhs);
		return cmp > 0 or cmp == 0 and v[0] >= rhs;
	}

	bool operator<=(dtype rhs) const {
		rhs = SafeForOperation(rhs);
		const int cmp = compare(rhs);
		return cmp < 0 or cmp == 0 and v[0] <= rhs;
	}

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
		ClampBits();
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
		ClampBits();
		return *this;
	}

	vint& operator*=(const vint& rhs) {
		static_assert(num_word == 1, "Multiplication > 64b is not supported");
		v[0] *= rhs.v[0];
		ClampBits();
		return *this;
	}

	vint& operator/=(const vint& rhs) {
		static_assert(num_word == 1, "Division > 64b is not supported");
		v[0] /= rhs.v[0];
		ClampBits();
		return *this;
	}

	vint& operator+=(const dtype rhs) {
		if constexpr (num_word == 1) {
			v[0] += rhs;
		} else {
			unsigned char carry = 0;
			carry = detail::addcarry64(v[0], v[0], rhs, carry);
			for (unsigned i = 1; i < num_word; ++i) {
				carry = detail::addcarry64(v[i], v[i], 0, carry);
			}
		}
		ClampBits();
		return *this;
	}

	vint& operator-=(const dtype rhs) {
		if constexpr (num_word == 1) {
			v[0] -= rhs;
		} else {
			unsigned char carry = 0;
			carry = detail::subborrow64(v[0], v[0], rhs, carry);
			for (unsigned i = 1; i < num_word; ++i) {
				carry = detail::subborrow64(v[i], v[i], 0, carry);
			}
		}
		ClampBits();
		return *this;
	}

	vint& operator*=(const dtype rhs) {
		static_assert(num_word == 1, "Multiplication > 64b is not supported");
		v[0] *= rhs;
		ClampBits();
		return *this;
	}

	vint& operator/=(const dtype rhs) {
		static_assert(num_word == 1, "Division > 64b is not supported");
		v[0] /= rhs;
		ClampBits();
		return *this;
	}

	//////////////////////
	// bitwise assignment
	//////////////////////
	vint& operator&=(const vint& rhs) {
		for (unsigned i = 0; i < num_word; ++i) {
			v[i] &= rhs[i];
		}
		// ClampBits();
		return *this;
	}

	vint& operator|=(const vint& rhs) {
		for (unsigned i = 0; i < num_word; ++i) {
			v[i] |= rhs[i];
		}
		// ClampBits();
		return *this;
	}

	vint& operator^=(const vint& rhs) {
		for (unsigned i = 0; i < num_word; ++i) {
			v[i] ^= rhs[i];
		}
		// ClampBits();
		return *this;
	}

	vint& operator&=(const dtype rhs) {
		v[0] &= rhs;
		ClampBits();
		return *this;
	}

	vint& operator|=(const dtype rhs) {
		v[0] |= rhs;
		ClampBits();
		return *this;
	}

	vint& operator^=(const dtype rhs) {
		v[0] ^= rhs;
		ClampBits();
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
		if constexpr (not is_signed) {
			ClampBits();
		}
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
			unsigned char carry = dtype(1);
			for (unsigned i = 0; i < num_word; ++i) {
				carry = detail::addcarry64(v[i], dtype(~v[i]), dtype(0), carry);
			}
		}
		if constexpr (not is_signed) {
			ClampBits();
		}
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
	template<unsigned num_bit_dst>
	explicit operator vint<is_signed, num_bit_dst>() const {
		vint<is_signed, num_bit_dst> dst;
		auto src_beg = ::std::begin(v);
		auto src_end = ::std::end(v);
		auto dst_beg = ::std::begin(dst.v);
		auto dst_end = ::std::end(dst.v);
		constexpr size_t num_word_min = detail::num_bit2num_word(::std::min(num_bit, num_bit_dst));
		::std::copy_n(src_beg, num_word_min, dst_beg);
		if constexpr (num_bit_dst > num_bit) {
			if constexpr (is_signed) {
				::std::fill(
					dst_beg+num_word_min,
					dst_end,
					dst_beg[num_word_min-1]>>(num_bit_dst-1)
				);
			} else {
				::std::fill(dst_beg+num_word_min, dst_end, 0);
			}
		} else {
			dst.ClampBits();
		}
		return dst;
	}

	//////////////////////
	// shift assignment
	//////////////////////
	vint& operator>>=(const unsigned rhs) {
		// rhs >= num_bit is UB in C, but not sure in Verilog
		if (rhs == 0 or rhs >= num_bit) {
			// do nothing
			return *this;
		}

		if constexpr (num_word == 1) {
			v[0] >>= rhs;
		} else {
			const dtype sign_ext = v[num_word-1] >> 63;
			const unsigned word_shift = rhs / 64;
			const unsigned bit_shift = rhs % 64;
			if (bit_shift == 0) {
				for (unsigned i = word_shift; i < num_word; ++i) {
					v[i-word_shift] = v[i];
				}
			} else {
				const unsigned interleave_bit = bit_shift;
				for (unsigned i = word_shift; i < num_word-1; ++i) {
					v[i-word_shift] = detail::interleave64(v[i], v[i+1], interleave_bit);
				}
				v[num_word-word_shift-1] = detail::interleave64(v[num_word-1], sign_ext, interleave_bit);
			}
			::std::fill_n(::std::end(v) - word_shift, word_shift, sign_ext);
		}
		// Not necessary for right shift
		// ClampBits();
		return *this;
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
			const dtype sign_ext = v[num_word-1] >> 63;
			unsigned word_shift = rhs / 64;
			unsigned bit_shift = rhs % 64;
			if (bit_shift == 0) {
				for (unsigned i = num_word; i > word_shift;) {
					--i;
					v[i] = v[i-word_shift];
				}
			} else {
				const unsigned interleave_bit = 64 - bit_shift;
				for (unsigned i = num_word; i > word_shift+1;) {
					--i;
					v[i] = detail::interleave64(v[i-word_shift-1], v[i-word_shift], interleave_bit);
				}
				v[word_shift] = detail::interleave64(dtype(0), v[0], interleave_bit);
			}
			::std::fill_n(::std::begin(v), word_shift, 0);
		}
		ClampBits();
		return *this;
	}

	template <bool is_signed2, unsigned num_bit2>
	vint& operator>>=(const vint<is_signed2, num_bit2>& rhs) {
		*this >>= rhs.value();
	}

	template <bool is_signed2, unsigned num_bit2>
	vint& operator<<=(const vint<is_signed2, num_bit2>& rhs) {
		*this <<= rhs.value();
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

	vint operator+(const dtype rhs) { vint ret = *this; ret += rhs; return ret; }
	vint operator-(const dtype rhs) { vint ret = *this; ret -= rhs; return ret; }
	vint operator*(const dtype rhs) { vint ret = *this; ret *= rhs; return ret; }
	vint operator/(const dtype rhs) { vint ret = *this; ret /= rhs; return ret; }
	vint operator&(const dtype rhs) { vint ret = *this; ret &= rhs; return ret; }
	vint operator|(const dtype rhs) { vint ret = *this; ret |= rhs; return ret; }
	vint operator^(const dtype rhs) { vint ret = *this; ret ^= rhs; return ret; }
	vint operator>>(const dtype rhs) { vint ret = *this; ret >>= rhs; return ret; }
	vint operator<<(const dtype rhs) { vint ret = *this; ret <<= rhs; return ret; }
	bool operator!=(const dtype rhs) const { return not (*this == rhs); }

	//////////////////////
	// slice
	//////////////////////
	// TODO: slice
	bool Bit(unsigned pos) const {
#ifndef NDEBUG
		assert(pos < num_bit);
#endif
		return bool((v[pos/bw_word] >> (pos%bw_word)) & dtype(1));
	}

	//////////////////////
	// others
	//////////////////////
	dtype value() const {
		return v[0];
	}

	friend void from_hex(vint &val, const ::std::string &s) {
		constexpr unsigned max_len = (num_bit + 3) / 4;
		constexpr dtype msb_mask = (1 << (num_bit % 4)) - 1;
		::std::fill_n(::std::begin(val.v), num_word, 0);
		unsigned str_pos = s.size()-1;
		unsigned put_pos = 0;
		int last_put = 0;
		// pos == size_t(-1) is safe according to the standard
		for (put_pos = 0; put_pos < 4*max_len and str_pos != -1; --str_pos) {
			const char c = s[str_pos];
			int to_put;
			if ('0' <= c and c <= '9') {
				to_put = c - '0';
			} else if ('a' <= c and c <= 'f') {
				to_put = c - 'a' + 10;
			} else if ('A' <= c and c <= 'F') {
				to_put = c - 'A' + 10;
			} else {
				continue;
			}
			val.PutDtypeAtBitPosClean(put_pos, to_put);
			if (to_put != 0) {
				last_put = to_put;
			}
			put_pos += 4;
		}
		val.ClampBits();

		// Handle negative & sign extension for strings started with -, ', -'
		str_pos = 0;
		bool do_negate = false;
		if (s.size() > str_pos and s[str_pos] == '-') {
			do_negate = true;
			++str_pos;
		}
		// only support sign extension for signed now
		if constexpr (is_signed)
		if (last_put != 0)
		if (s.size() > str_pos and s[str_pos] == '\'') {
			const unsigned filled_bit =
				+ put_pos
				- unsigned(last_put < 8)
				- unsigned(last_put < 4)
				- unsigned(last_put < 2);
			if (filled_bit < num_bit) {
				const unsigned unfilled_bit = num_bit - filled_bit;
				val <<= unfilled_bit;
				val.ClampBits();
				val >>= unfilled_bit;
			}
		}
		if (do_negate) {
			val.Negate();
		}
	}

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

	friend ::std::ostream& operator<<(::std::ostream& os, const vint &v) {
		if constexpr (num_word == 1) {
			os << +v.value();
		} else {
			os << to_hex(v);
		}
		return os;
	}

};

template<unsigned num_bit> using vsint = vint<true, num_bit>;
template<unsigned num_bit> using vuint = vint<false, num_bit>;

} // namespace verilog
