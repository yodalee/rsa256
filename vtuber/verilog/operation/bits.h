#pragma once
// Direct include
// C system headers
// C++ standard library headers
#include <utility>
// Other libraries' .h files.
// Your project's .h files.
#include "verilog/dtype_base.h"

namespace verilog {

///////////////////
// analog to verilog $bits
// an abstraction layer to calculate the number of bits of a datatype
// also put forward declaration here
///////////////////
template<typename T_> constexpr unsigned bits();

namespace detail {

// need forward declaration since there might be nested struct/union
template<typename T, unsigned ...i>
constexpr unsigned vstruct_bits(::std::integer_sequence<unsigned, i...>);
// template<typename T, unsigned ...i>
// constexpr unsigned vunion_bits(::std::integer_sequence<unsigned, i...>);

// implement
template<typename T, unsigned ...i>
constexpr unsigned vstruct_bits(::std::integer_sequence<unsigned, i...>) {
	T* tp = nullptr;
	return (bits<decltype(tp->template get<i>())>() + ...);
}

} // namespace detail

template<typename T_> constexpr unsigned bits() {
	typedef ::std::remove_reference_t<T_> T;
	static_assert(is_dtype_v<T>, "T must be a verilog type");
	if constexpr (is_vint_v<T>) {
		return T::num_bit;
	} else if constexpr (is_varray_v<T>) {
		return bits<typename T::dtype>() * T::asize;
	} else if constexpr (is_vstruct_v<T>) {
		return detail::vstruct_bits<T>(::std::make_integer_sequence<unsigned, T::num_members>());
//	} else if constexpr (is_vunion_v<T>) {
//		return detail::vunion_bits<T>(::std::make_integer_sequence<unsigned, T::num_member>());
	}
}
template<typename T> inline constexpr unsigned bits_v = bits<T>();

} // namespace verilog
