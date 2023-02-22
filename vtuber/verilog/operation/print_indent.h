#pragma once
// Direct include
// C system headers
// C++ standard library headers
#include <ostream>
#include <utility>
// Other libraries' .h files.
// Your project's .h files.
#include "verilog/dtype_base.h"

namespace verilog {

template<typename T> void print_indent(const T& t);

namespace detail {

void put_indent(::std::ostream& ost, unsigned level) {
	for (unsigned i = 0; i < level; ++i) {
		ost.put('\t');
	}
}

// need forward declaration since there might be nested vuint/array/struct/union
template<typename T, unsigned ...i>
auto vstruct_print_indent(
	::std::ostream& ost, unsigned level, const T& t,
	::std::integer_sequence<unsigned, i...> idx
);
// template<typename T, unsigned ...i>
// auto vunion_pack(
// 	const T &src,
// 	::std::integer_sequence<unsigned, i...> idx
// );

template<typename T, unsigned ...i>
auto vstruct_print_indent(
	::std::ostream& ost, unsigned level, const T& t,
	::std::integer_sequence<unsigned, i...> idx
) {
	(
		(
			put_indent(ost, level),
			(ost << T::get_name(i)),
			(print_indent(ost, level+1u, t.template get<i>())),
			(ost << "\n")
		),
	...);
}

} // namespace detail

template<typename T>
void print_indent(::std::ostream& ost, unsigned level, const T& t) {
	static_assert(is_dtype_v<T>, "T must be a verilog type");
	if constexpr (is_vint_v<T> or is_varray_v<T>) {
		ost << ": " << t;
	} else if constexpr (is_vstruct_v<T>) {
		if (level != 0) {
			ost << ":\n";
		}
		detail::vstruct_print_indent<T>(
			ost, level, t,
			::std::make_integer_sequence<unsigned, T::num_members>()
		);
//	} else if constexpr (is_vunion_v<T>) {
//		return vint<false, 0u>();
	}
}

} // namespace verilog
