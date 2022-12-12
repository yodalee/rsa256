#pragma once
#include "verilog_int.h"
#include <boost/hana/define_struct.hpp>
#include <boost/hana/fold.hpp>
#include <boost/hana/for_each.hpp>
// for memcmp
#include <cstring>
#include <iostream>
#include <sstream>
#include <string>
#include <type_traits>

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

namespace hana = boost::hana;
template <class T, unsigned... dims>
struct varray {
	static_assert(::std::is_trivially_copyable_v<T>);
	static_assert(sizeof...(dims) > 0);

	// array type/size, data type
	typedef detail::get_array_type<T, dims...> atype;
	static constexpr unsigned asize = (dims * ...);
	typedef T dtype;

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

///////////////////
// detail::num_bit_of_
///////////////////
template <bool is_signed, unsigned num_bit>
constexpr unsigned num_bit_of_(vint<is_signed, num_bit>) {
	return num_bit;
};

template<class T> constexpr unsigned num_bit_of_(T);
template <class T, unsigned... dims>
constexpr unsigned num_bit_of_(varray<T, dims...>) {
	constexpr unsigned base_size = num_bit_of_(T{});
	return (base_size * ... * dims);
};

template<class T>
constexpr unsigned num_bit_of_(T) {
	return hana::fold(
		hana::accessors<T>(), 0u,
		[](unsigned cur, const auto& accessor) {
			typedef decltype(hana::second(accessor)(T{})) TSub;
			return cur + num_bit_of_(
				// create an object of one member to calculate the #bit recursively
				TSub{}
			);
		}
	);
};

} // namespace detail

namespace detail {

///////////////////
// detail::PrintString_
///////////////////
template <bool is_signed, unsigned num_bit>
::std::ostream& PrintTypeString_(::std::ostream &os, vint<is_signed, num_bit>*) {
	return os << (is_signed ? "s" : "u") << num_bit;
};

template <class T> ::std::ostream& PrintTypeString_(::std::ostream &os, T*);
template <class T, unsigned... dims>
::std::ostream& PrintTypeString_(::std::ostream &os, varray<T, dims...>*) {
	os << "[";
	::std::remove_reference_t<T>* tp{};
	PrintTypeString_(os, tp);
	os << "]";
	(os << ... << ("x" + ::std::to_string(dims)));
	return os;
};

template <class T>
::std::ostream& PrintTypeString_(::std::ostream &os, T*) {
	os << "{";
	hana::for_each(
		hana::accessors<T>(),
		[&](const auto &accessor) {
			::std::remove_reference_t<decltype(hana::second(accessor)(T{}))>* tp{};
			PrintTypeString_(os, tp);
		}
	);
	os << "}";
	return os;
};

} // namespace detail

template <bool is_signed, unsigned num_bit>
::std::ostream& PrintContent(::std::ostream &os, const vint<is_signed, num_bit> &rhs) {
	return os << rhs;
};

template <class T, unsigned... dims>
::std::ostream& PrintContent(::std::ostream &os, const varray<T, dims...>& rhs) {
	return os << rhs;
};

template <class T>
::std::ostream& PrintContent(::std::ostream &os, const T& rhs) {
	os << "{";
	hana::for_each(
		hana::accessors<T>(),
		[&](const auto &accessor) {
			os << hana::first(accessor).c_str() << ":";
			PrintContent(os, hana::second(accessor)(rhs));
			os << ",";
		}
	);
	os << "}";
	return os;
};

template <class T>
::std::ostream& PrintTypeString(::std::ostream &os) {
	T* tp{};
	return detail::PrintTypeString_(os, tp);
};

template <class T>
::std::string GetTypeString() {
	::std::stringstream ss;
	T* tp{};
	detail::PrintTypeString_(ss, tp);
	::std::string ret;
	ss >> ret;
	return ret;
};

template <class T> constexpr unsigned num_bit_of = detail::num_bit_of_(T{});

} // namespace verilog

// Shall not use memcmp after shifting to C++20, which has default operator==()
#define VERILOG_DEFINE_STRUCT_EXTRA(struct_name) \
	friend ::std::ostream& operator<<(::std::ostream &os, const struct_name& rhs) {\
		return ::verilog::PrintContent(os, rhs);\
	}\
	bool operator==(const struct_name& rhs) const {\
		return memcmp(this, &rhs, sizeof(struct_name)) == 0;\
	}\
	bool operator!=(const struct_name& rhs) const { return not (*this == rhs); }
