#pragma once
#include "verilog_int.h"
#include <boost/hana/define_struct.hpp>
#include <boost/hana/fold.hpp>
#include <ostream>
#include <type_traits>
namespace hana = boost::hana;

namespace verilog {

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

template <class T> constexpr unsigned num_bit_of = detail::num_bit_of_(T{});

/*
template <class T>
::std::ostream& PrintHana(::std::ostream &os, const T& rhs) {
	hana::for_each(
		hana::accessors<T>(),
		[&](const auto &accessor) {
			os << hana::secondaccessor(rhs) << std::endl;
		}
	);
};
*/

} // namespace verilog
