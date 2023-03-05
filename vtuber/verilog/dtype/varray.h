#pragma once
// Direct include
// C system headers
// C++ standard library headers
#include <type_traits>
// Other libraries' .h files.
// Your project's .h files.
#include "verilog/dtype_base.h"
#include "verilog/dtype/vint.h"
#include "verilog/operation/print_indent.h"

namespace verilog {

namespace detail {

///////////////////
// detail::get_array_type
///////////////////
template <class T>
struct get_array_type_helper {
	typedef T type;
};

template<typename T, unsigned X>
auto operator*(::std::integral_constant<unsigned, X>, get_array_type_helper<T>) {
	return get_array_type_helper<T[X]>();
}

template <class T, unsigned... dims>
auto get_array_type() {
	return (
		::std::integral_constant<unsigned, dims>()
		* ... *
		(get_array_type_helper<T>())
	);
};

static inline void advance_array_idx(
	unsigned *array_idx, const unsigned *array_bound, const unsigned array_dim,
	unsigned& num_resulting_trailing_zero, unsigned& flat_index
) {
	num_resulting_trailing_zero = 0;
	unsigned cur_flat_stride = 1;
	for (unsigned i = array_dim-1;; --i) {
		array_idx[i] += 1;
		flat_index += cur_flat_stride;
		if (array_idx[i] == array_bound[i]) {
			cur_flat_stride *= array_bound[i];
			array_idx[i] = 0;
			flat_index -= cur_flat_stride;
			++num_resulting_trailing_zero;
		} else {
			break;
		}
		if (i == 0) {
			break;
		}
	}
}

} // namespace detail

template <class T, unsigned... dims>
struct varray {
	// Make template arguments accessible from outside
	typedef typename decltype(detail::get_array_type<T, dims...>())::type atype;
	static constexpr unsigned asize = (dims * ...);
	static constexpr unsigned ndim = sizeof...(dims);
	typedef T dtype;
	static_assert(ndim > 0);
	// Make array compatible to dtype
	TAG_AS_VARRAY

	// declate the actual data
	atype v;

	T* begin() { return reinterpret_cast<T*>(&v); }
	T* end  () { return begin() + asize; }
	const T* begin() const { return reinterpret_cast<const T*>(&v); }
	const T* end  () const { return begin() + asize; }
	auto operator[](unsigned i) { return v[i]; }

	void print_with_helper(detail::ost_indent_helper &helper) const {
		static const unsigned array_bound[ndim] { dims... };
		unsigned array_idx[ndim] {};
		unsigned pending_indent_level = ndim, flat_idx = 0;
		const T* flat_ptr = begin();
		while (true) {
			unsigned num_resulting_trailing_zero;
			for (unsigned i = 0; i < pending_indent_level; ++i) {
				const bool inline_mode = (i+1) == pending_indent_level;
				helper.put_newitem();
				helper.open_indent('[', inline_mode);
			}
			helper.put_newitem();
			helper << flat_ptr[flat_idx];
			detail::advance_array_idx(
				array_idx, array_bound, ndim,
				num_resulting_trailing_zero, flat_idx
			);
			for (unsigned i = 0; i < num_resulting_trailing_zero; ++i) {
				helper.close_indent(']');
			}
			pending_indent_level = num_resulting_trailing_zero;
			if (num_resulting_trailing_zero == ndim) {
				break;
			}
		}
	}

	friend ::std::ostream& operator<<(::std::ostream& os, const varray &rhs) {
		detail::ost_indent_helper helper(os);
		rhs.print_with_helper(helper);
		return os;
	}
};

} // namespace verilog
