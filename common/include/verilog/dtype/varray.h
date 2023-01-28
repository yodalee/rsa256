#pragma once
// Direct include
// C system headers
// C++ standard library headers
#include <type_traits>
// Other libraries' .h files.
// Your project's .h files.
#include "verilog/dtype_base.h"
#include "verilog/dtype/vint.h"

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

template<unsigned ret_bits, typename T, unsigned ...i>
vint<false, ret_bits> varray_Packed(
	const T* tp,
	::std::integer_sequence<unsigned, i...> idx
) {
	constexpr unsigned max_idx = idx.size()-1;
	constexpr unsigned bit_of_T = T::bits();
	vuint<ret_bits> ret;
	::std::fill(::std::begin(ret.v), ::std::end(ret.v), 0);
	(ret.template WriteSliceUnsafe<i*bit_of_T, bit_of_T>(tp[max_idx-i].Packed()), ...);
	return ret;
}

} // namespace detail

template <class T, unsigned... dims>
struct varray {
	// Make template arguments accessible from outside
	typedef typename decltype(detail::get_array_type<T, dims...>())::type atype;
	static constexpr unsigned asize = (dims * ...);
	typedef T dtype;
	static_assert(sizeof...(dims) > 0);
	// Make array compatible to dtype
	static constexpr unsigned dtype_tag = DTYPE_VARRAY;
	static constexpr unsigned bits() { return T::bits() * asize; }

	// declate the actual data
	atype v;

	T* begin() { return reinterpret_cast<T*>(&v); }
	T* end  () { return begin() + asize; }
	const T* begin() const { return reinterpret_cast<const T*>(&v); }
	const T* end  () const { return begin() + asize; }
	auto operator[](unsigned i) { return v[i]; }
	auto Packed() const {
		return verilog::detail::varray_Packed<bits()>(
			begin(),
			::std::make_integer_sequence<unsigned, asize>()
		);
	}

	friend ::std::ostream& operator<<(::std::ostream& os, const varray &rhs) {
		os << "[";
		for (auto t: rhs) {
			os << t << ",";
		}
		os << "]";
		return os;
	}
};

} // namespace verilog
