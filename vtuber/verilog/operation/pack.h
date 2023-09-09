#pragma once
// Direct include
// C system headers
// C++ standard library headers
#include <utility>
// Other libraries' .h files.
// Your project's .h files.
#include "verilog/dtype_base.h"
#include "verilog/dtype/vint.h"
#include "verilog/operation/bits.h"

namespace verilog {

///////////////////
// analog to verilog {>>{}} (packing)
// an abstraction layer to pack a datatype
// also put forward declaration here
///////////////////
template<typename T> vint<false, bits<T>()> pack(const T& t);

namespace detail {

// need forward declaration since there might be nested vuint/array/struct/union
template<typename T, unsigned ...i>
auto varray_pack(
	const T &src,
	::std::integer_sequence<unsigned, i...> idx
);
template<typename T, unsigned ...i>
auto vstruct_pack(
	const T &src,
	::std::integer_sequence<unsigned, i...> idx
);
// template<typename T, unsigned ...i>
// auto vunion_pack(
// 	const T &src,
// 	::std::integer_sequence<unsigned, i...> idx
// );

template<typename T>
auto vint_pack(const T &src) {
	vint<false, T::num_bit> dst;
	::std::fill(::std::begin(dst.v), ::std::end(dst.v), 0);
	::std::copy(::std::begin(src.v), ::std::end(src.v), ::std::begin(dst.v));
	return dst;
}

template<typename T, unsigned ...i>
auto varray_pack(
	const T &src,
	::std::integer_sequence<unsigned, i...> idx
) {
	typedef typename T::dtype dtype;
	vint<false, bits<T>()> dst;
	constexpr unsigned bit_of_T = bits<dtype>();
	const dtype *tp = src.begin();
	::std::fill(::std::begin(dst.v), ::std::end(dst.v), 0);
	(dst.template WriteSliceUnsafe<i*bit_of_T, bit_of_T>(pack(tp[i])), ...);
	return dst;
}

template<typename T, unsigned ...i>
auto vstruct_pack(
	const T &src,
	::std::integer_sequence<unsigned, i...> idx
) {
	return concat(pack(src.template get<i>())...);
}

} // namespace detail

template<typename T>
vint<false, bits<T>()> pack(const T& t) {
	static_assert(is_dtype_v<T>, "T must be a verilog type");
	if constexpr (is_vint_v<T>) {
		return detail::vint_pack<T>(t);
	} else if constexpr (is_varray_v<T>) {
		return detail::varray_pack<T>(t, ::std::make_integer_sequence<unsigned, T::asize>());
	} else if constexpr (is_vstruct_v<T>) {
		return detail::vstruct_pack<T>(t, ::std::make_integer_sequence<unsigned, T::num_members>());
//	} else if constexpr (is_vunion_v<T>) {
//		return vint<false, 0u>();
	}
}

} // namespace verilog
