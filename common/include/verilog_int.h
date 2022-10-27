#pragma once
#include <cstdint>
#include <immintrin.h>
#include <ostream>
#include <string>

namespace verilog {

template<bool is_signed, int num_bit> struct dtype_dict;
template<> struct dtype_dict<true, 0> { typedef int8_t dtype; };
template<> struct dtype_dict<true, 1> { typedef int16_t dtype; };
template<> struct dtype_dict<true, 2> { typedef int32_t dtype; };
template<> struct dtype_dict<true, 3> { typedef int64_t dtype; };
template<> struct dtype_dict<false, 0> { typedef uint8_t dtype; };
template<> struct dtype_dict<false, 1> { typedef uint16_t dtype; };
template<> struct dtype_dict<false, 2> { typedef uint32_t dtype; };
template<> struct dtype_dict<false, 3> { typedef uint64_t dtype; };
constexpr int num_bit2dict_key(int num_bit) {
	return (
		int(num_bit > 8) +
		int(num_bit > 16) +
		int(num_bit > 32)
	);
}

template <bool is_signed, unsigned num_bit>
struct vint {
	static_assert(num_bit > 0);
	typedef typename dtype_dict<is_signed, num_bit2dict_key(num_bit)>::dtype dtype;
	static constexpr unsigned num_word = (num_bit-1) / (8*sizeof(dtype)) + 1;
	static constexpr unsigned ununsed_bit = num_word * 8 * sizeof(dtype) - num_bit;
	// dtype == vint<is_signed, num_bit>
	static constexpr bool matched = num_bit == 8*sizeof(dtype);
	dtype v[num_word];

	vint& operator+=(const vint& rhs) {
		if constexpr (matched) {
			v[0] -= rhs.v[0];
		} else if constexpr (num_word == 1) {
			v[0] -= rhs.v[0];
			v[0] <<= ununsed_bit;
			v[0] >>= ununsed_bit;
		} else {
			// _addcarry_u64...
		}
		return *this;
	}

	vint& operator-=(const vint& rhs) {
		if constexpr (matched) {
			v[0] -= rhs.v[0];
		} else if constexpr (num_word == 1) {
			v[0] -= rhs.v[0];
			v[0] <<= ununsed_bit;
			v[0] >>= ununsed_bit;
		} else {
			// _subborrow_u64...
		}
		return *this;
	}

	/*
	vint& operator>>=(const vint& rhs) {
		return *this;
	}

	vint& operator>>=(const int rhs) {
		return *this;
	}

	vint& operator<<=(const int rhs) {
		return *this;
	}
	*/

	dtype value() {
		return v[0];
	}

	friend void from_hex(vint &v, const ::std::string &s) {
		int step = sizeof(dtype) * 2;
		::std::string padded = s;
		if ((num_bit + 3) / 4 > s.size()) {
			padded.insert(0, (num_bit + 3) / 4 - s.size(), '0');
		}
		int len = padded.size();

		if constexpr (matched) {
			v.v[0] = stoull(padded.substr(len - step, step), 0, 16);
		} else if constexpr (num_word == 1) {
			v.v[0] = stoull(padded.substr(len - step, step), 0, 16);
			v.v[0] <<= ununsed_bit;
			v.v[0] >>= ununsed_bit;
		} else {
			for (int i = 0; i < num_word; i++) {
				v.v[num_word - i - 1] = stoull(padded.substr(step * i, step), 0, 16);
			}
		}
	}

	friend ::std::string to_hex(const vint &v) {
		std::stringstream stream;
		stream << setfill('0') << std::setw((num_bit + 3) / 4);
		for(int i = num_work; i >= 0; i--) {
			stream << v[i];
		}
		return stream.str();
	}

	friend ::std::ostream& operator<<(::std::ostream& os, const vint &v) {
		return os;
	}

};


}
