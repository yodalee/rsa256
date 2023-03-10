#pragma once
// Direct include
// C system headers
// C++ standard library headers
#include <array>
#include <cstdint>
#include <cassert>
#include <iomanip>
#include <ostream>
#include <string>
#include <type_traits>
// Other libraries' .h files.
// Your project's .h files.
#include "verilog/dtype_base.h"

namespace verilog {

namespace detail {

template<bool is_signed, unsigned num_bit> struct dtype_dict;
template<> struct dtype_dict<true, 0u> { typedef int8_t type; };
template<> struct dtype_dict<true, 1u> { typedef int16_t type; };
template<> struct dtype_dict<true, 2u> { typedef int32_t type; };
template<> struct dtype_dict<true, 3u> { typedef int64_t type; };
template<> struct dtype_dict<false, 0u> { typedef uint8_t type; };
template<> struct dtype_dict<false, 1u> { typedef uint16_t type; };
template<> struct dtype_dict<false, 2u> { typedef uint32_t type; };
template<> struct dtype_dict<false, 3u> { typedef uint64_t type; };
// compute required indices for uint8/16/32/64
constexpr unsigned num_bit2dict_key(unsigned num_bit) {
	return (
		unsigned(num_bit > 8) +
		unsigned(num_bit > 16) +
		unsigned(num_bit > 32)
	);
}
// promoted type
template<unsigned num_bit> using ptype = typename dtype_dict<
	false,
	2u + unsigned(num_bit > 32)
>::type;
// storage type
template<unsigned num_bit> using stype = typename dtype_dict<
	false,
	num_bit2dict_key(num_bit)
>::type;
// casted type
template<bool is_signed, unsigned num_bit> using ctype = typename dtype_dict<
	is_signed,
	num_bit2dict_key(num_bit)
>::type;

static void from_hex(
	const char* str, uint64_t& value, unsigned base,
	bool& is_signed, bool &sign_ext
) {
}

} // namespace detail

template <
	bool is_signed_,
	unsigned num_bit_,
	bool is_multiple_word_ = (num_bit_>64u)
> struct vint2;

//////////////////////////////////////////
// Single word implementation
//////////////////////////////////////////
template <bool is_signed_, unsigned num_bit_>
struct vint2<is_signed_, num_bit_, false> {
	// capture template parameters
	static constexpr bool is_signed = is_signed_;
	static constexpr unsigned num_bit = num_bit_;
	// promoted type
	typedef typename detail::ptype<num_bit> ptype;
	static constexpr unsigned psize = 8*sizeof(ptype);
	// storage type
	typedef typename detail::stype<num_bit> stype;
	// casted type
	typedef typename detail::ctype<is_signed, num_bit> ctype;
	// constexprs for computation
	static constexpr ptype lo_mask = ptype(-1) >> (psize-num_bit);
	static constexpr ptype msb_mask = lo_mask ^ (lo_mask>>1);

	// the only storage of vint
	stype v;

private:
	void ClearUnusedBits() {
		v = v & lo_mask;
	}

public:
	vint2() = default;
	vint2(vint2 const& rhs) = default;
	vint2(ptype rhs) {
		v = rhs;
		ClearUnusedBits();
	}
	vint2(const ::std::string& s, unsigned base) {
	}
};

//////////////////////////////////////////
// Multiple word implementation
//////////////////////////////////////////
template <bool is_signed_, unsigned num_bit_>
struct vint2<is_signed_, num_bit_, true> {
	// capture template parameters
	static constexpr bool is_signed = is_signed_;
	static constexpr unsigned num_bit = num_bit_;
	// promoted type
	typedef typename detail::ptype<num_bit> ptype;
	static constexpr unsigned psize = 8*sizeof(ptype);
	// storage type
	typedef typename detail::stype<num_bit> stype;
	// casted type
	typedef typename detail::ctype<is_signed, num_bit> ctype;
	// constexprs for computation
	static constexpr unsigned num_word = (num_bit+psize-1) / psize;
	static constexpr ptype lo_mask = ptype(-1) >> ((-num_bit)%psize);
	static constexpr ptype msb_mask = lo_mask ^ (lo_mask>>1);

	// the only storage of vint
	stype v[num_word];

private:
	void ClearUnusedBits() {
		v[num_word-1] = v[num_word-1] & lo_mask;
	}

public:
	vint2() = default;
	vint2(vint2 const& rhs) = default;
	vint2(ptype rhs) {
		v[0] = rhs;
		const bool is_neg = is_signed and (rhs&msb_mask) != 0;
		const ptype fillv = ptype(is_neg ? -1 : 0);
		::std::fill(rhs.begin()+1, rhs.end(), fillv);
		ClearUnusedBits();
	}
	vint2(const ::std::array<ptype, num_word>& rhs) {
		::std::copy(rhs.begin(), rhs.end(), v);
		ClearUnusedBits();
	}
	vint2(const ::std::string& s, unsigned base) {
	}

};

template<unsigned num_bit> using vsint2 = vint2<true, num_bit>;
template<unsigned num_bit> using vuint2 = vint2<false, num_bit>;

} // namespace verilog
