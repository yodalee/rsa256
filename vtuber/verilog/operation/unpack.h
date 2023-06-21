#pragma once
// Direct include
// C system headers
// C++ standard library headers
#include <utility>
// Other libraries' .h files.
// Your project's .h files.
#include "verilog/dtype_base.h"
#include "verilog/operation/bits.h"
#include "verilog/operation/bit_offset.h"

namespace verilog {

namespace detail {

///////////////////
// analog to verilog {>>{}} (unpacking)
// an abstraction layer to unpack a datatype
// also put forward declaration here
///////////////////
template<typename T, unsigned num_bit>
void unpack_nocheck(T& t, const vint<false, num_bit> u);

// need forward declaration since there might be nested vuint/array/struct/union
template<typename T, unsigned num_bit, unsigned ...i>
void varray_unpack(
	T& t, const vint<false, num_bit> u,
	::std::integer_sequence<unsigned, i...> idx
);
template<typename T, unsigned num_bit, unsigned ...i>
void vstruct_unpack(
	T& t, const vint<false, num_bit> u,
	::std::integer_sequence<unsigned, i...> idx
);
// template<typename T, unsigned num_bit, unsigned ...i>
// void vunion_unpack(
// 	const vint<num_bit, false> u, T& t,
// 	::std::integer_sequence<unsigned, i...> idx
// );

template<typename T, unsigned num_bit, unsigned ...i>
void varray_unpack(
	T& t, const vint<false, num_bit> u,
	::std::integer_sequence<unsigned, i...> idx
) {
	constexpr unsigned num_bit_element = bits_v<typename T::dtype>;
	typename T::dtype* t_ptr = t.begin();
	((
		unpack_nocheck(
			t_ptr[i],
			u.template Slice<(T::asize-1-i)*num_bit_element, num_bit_element>()
		)
	),...);
}

template<typename T, unsigned num_bit, unsigned ...i>
void vstruct_unpack(
	T& t, const vint<false, num_bit> u,
	::std::integer_sequence<unsigned, i...> idx
) {
	((
		unpack_nocheck(
			t.template get<i>(),
			u.template Slice<
				bit_offset_first_member_at_msb_v<T, i>,
				bits_v<decltype(t.template get<i>())>
			>()
		)
	),...);
}

template<typename T, unsigned num_bit>
void unpack_nocheck(T& t, const vint<false, num_bit> u) {
	static_assert(is_dtype_v<T>, "T must be a verilog type");
	if constexpr (is_vint_v<T>) {
		t = u;
	} else if constexpr (is_varray_v<T>) {
		detail::varray_unpack<T>(t, u, ::std::make_integer_sequence<unsigned, T::asize>());
	} else if constexpr (is_vstruct_v<T>) {
		detail::vstruct_unpack<T>(t, u, ::std::make_integer_sequence<unsigned, T::num_members>());
//	} else if constexpr (is_vunion_v<T>) {
	}
}

} // namespace detail

template<typename T, unsigned num_bit>
void unpack(T& t, const vint<false, num_bit>& u) {
	static_assert(bits_v<T> == num_bit, "Unpack and vint must have the same bitwidth");
	detail::unpack_nocheck(t, u);
}

} // namespace verilog
