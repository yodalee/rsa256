#pragma once
#include "verilog_int.h"
#include <boost/hana/define_struct.hpp>
#include <boost/hana/fold.hpp>
#include <boost/hana/for_each.hpp>
// for memcmp
#include <cstring>
#include <iostream>
#include <string>
#include <type_traits>

namespace verilog {

namespace hana = boost::hana;

namespace detail {

template <bool is_signed, unsigned num_bit>
constexpr unsigned num_bit_of_(vint<is_signed, num_bit>) {
	return num_bit;
};

template<class T>
constexpr unsigned num_bit_of_(T) {
	return hana::fold(
		hana::accessors<T>(), 0u,
		[](unsigned cur, const auto& accessor) {
			return cur + num_bit_of_(
				// create an object of one member to calculate the #bit recursively
				decltype(hana::second(accessor)(T{})){}
			);
		}
	);
};

} // namespace detail

template <bool is_signed, unsigned num_bit>
::std::ostream& PrintContent(::std::ostream &os, const vint<is_signed, num_bit> &rhs) {
	return (os << rhs << ",");
};

template <class T>
::std::ostream& PrintContent(::std::ostream &os, const T& rhs) {
	os << "{";
	hana::for_each(
		hana::accessors<T>(),
		[&](const auto &accessor) {
			os << hana::first(accessor).c_str() << ":";
			PrintContent(os, hana::second(accessor)(rhs));
		}
	);
	os << "},";
	return os;
};

template <class T>
::std::ostream& SaveContent(::std::ostream &os, const T& rhs) {
	static_assert(::std::is_trivially_copyable_v<T>);
	os.write(reinterpret_cast<const char*>(&rhs), sizeof(T));
	return os;
};

template <class T>
::std::istream& LoadContent(::std::istream &ist, T& rhs) {
	static_assert(::std::is_trivially_copyable_v<T>);
	ist.read(reinterpret_cast<char*>(&rhs), sizeof(T));
	return ist;
};

template <bool is_signed, unsigned num_bit>
::std::string GetTypeString(const vint<is_signed, num_bit>&) {
	return ::std::string(is_signed ? "s" : "u") + ::std::to_string(num_bit);
};

template <class T>
::std::string GetTypeString(const T& t) {
	::std::string ret;
	hana::for_each(
		hana::accessors<T>(),
		[&](const auto &accessor) {
			ret += GetTypeString(hana::second(accessor)(t));
		}
	);
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
