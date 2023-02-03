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

	// declate the actual data
	atype v;

	T* begin() { return reinterpret_cast<T*>(&v); }
	T* end  () { return begin() + asize; }
	const T* begin() const { return reinterpret_cast<const T*>(&v); }
	const T* end  () const { return begin() + asize; }
	auto operator[](unsigned i) { return v[i]; }

	friend ::std::ostream& operator<<(::std::ostream& os, const varray &rhs) {
		os << "[";
		for (auto t: rhs) {
			os << t << ",";
		}
		os << "]";
		return os;
	}
};

namespace detail {

template<typename T, unsigned ...i>
auto varray_packed(
	const T &src,
	::std::integer_sequence<unsigned, i...> idx
) {
	typedef typename T::dtype dtype;
	vint<false, bits<T>()> dst;
	constexpr unsigned max_idx = idx.size()-1;
	constexpr unsigned bit_of_T = bits<dtype>();
	const dtype *tp = src.begin();
	::std::fill(::std::begin(dst.v), ::std::end(dst.v), 0);
	(dst.template WriteSliceUnsafe<i*bit_of_T, bit_of_T>(packed(tp[max_idx-i])), ...);
	return dst;
}

} // namespace detail

} // namespace verilog
