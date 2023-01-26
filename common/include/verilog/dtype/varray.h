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
	static constexpr unsigned bit_of_T = verilog::bits<T>();
	typedef T dtype;
	static_assert(::std::is_trivially_copyable_v<T>);
	static_assert(sizeof...(dims) > 0);
	// Make array compatible to dtype
	static constexpr unsigned dtype_tag = DTYPE_VARRAY;
	static constexpr unsigned bits() { return bit_of_T * asize; }

	// declate the actual data
	atype v;

	T* begin() { return reinterpret_cast<T*>(&v); }
	T* end  () { return begin() + asize; }
	const T* begin() const { return reinterpret_cast<const T*>(&v); }
	const T* end  () const { return begin() + asize; }
	auto operator[](unsigned i) { return v[i]; }
	template<unsigned...i>
	vuint<bits()> Packed(::std::integer_sequence<unsigned, i...>) const {
		vuint<bits()> ret;
		::std::fill(::std::begin(ret.v), ::std::end(ret.v), 0);
		const T* beg = begin();
		(ret.template WriteSliceUnsafe<i*bit_of_T, bit_of_T>(beg[asize-1-i].Packed()), ...);
		return ret;
	}
	vuint<bits()> Packed() const {
		return Packed(::std::make_integer_sequence<unsigned, asize>());
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
