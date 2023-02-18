#pragma once
// Direct include
// C system headers
// C++ standard library headers
#include <string>
#include <type_traits>
// Other libraries' .h files.
// Your project's .h files.
#include "namedtuple/namedtuple.h"
#include "verilog/dtype_base.h"
#include "verilog/dtype/vint.h"

namespace verilog {
namespace detail {

template<typename T, unsigned ...i>
constexpr unsigned vstruct_bits(::std::integer_sequence<unsigned, i...>) {
	T* tp = nullptr;
	return (bits<decltype(tp->template get<i>())>() + ...);
}

template<typename T, unsigned ...i>
auto vstruct_packed(
	const T &src,
	::std::integer_sequence<unsigned, i...> idx
) {
	return concat(packed(src.template get<i>())...);
}

} // namespace detail
} // namespace verilog

#define MAKE_VSTRUCT(...) MAKE_NAMEDTUPLE(__VA_ARGS__) TAG_AS_VSTRUCT
