#pragma once
// Direct include
// C system headers
// C++ standard library headers
#include <type_traits>
// Other libraries' .h files.
// Your project's .h files.

namespace verilog {

namespace detail {

///////////////////
// detail::get_array_type
///////////////////
template <class T, unsigned... dims> struct get_array_type;

template <class T>
struct get_array_type<T> {
	typedef T type;
};

template <class T, unsigned first_dim, unsigned... dims>
struct get_array_type<T, first_dim, dims...> {
	typedef typename get_array_type<T[first_dim], dims...>::type type;
};

} // namespace detail

template <class T, unsigned... dims>
struct varray {
	// Make template arguments accessible from outside
	typedef detail::get_array_type<T, dims...> atype;
	static constexpr unsigned asize = (dims * ...);
	typedef T dtype;
	static_assert(::std::is_trivially_copyable_v<T>);
	static_assert(sizeof...(dims) > 0);
	// Make array compatible to dtype
	static constexpr unsigned dtype_tag = DTYPE_VARRAY;
	static constexpr unsigned bits() { return bits<T>() * asize; }

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

} // namespace verilog
