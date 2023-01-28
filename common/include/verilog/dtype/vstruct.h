#pragma once
// Direct include
// C system headers
// C++ standard library headers
#include <string>
#include <type_traits>
// Other libraries' .h files.
// Your project's .h files.
#include "verilog/dtype_base.h"
#include "verilog/dtype/vint.h"

namespace verilog {
namespace detail {

template<typename T, unsigned ...i>
constexpr unsigned vstruct_bits(::std::integer_sequence<unsigned, i...>) {
	T* tp = nullptr;
	return (::std::remove_reference_t<decltype(tp->template get<i>())>::bits() + ...);
}

template<unsigned ret_bits, typename T, unsigned ...i>
vint<false, ret_bits> vstruct_Packed(
	const T& t,
	::std::integer_sequence<unsigned, i...> idx
) {
	return Concat((t.template get<i>().Packed())...);
}

} // namespace detail
} // namespace verilog

#define DEFINE_VSTRUCT(name) \
struct name {\
	static constexpr unsigned kCounterBegin = __COUNTER__+1;\
	using Str = ::std::string;\
	template<unsigned x> using Constant = ::std::integral_constant<unsigned, x>;\
	static constexpr unsigned dtype_tag = verilog::DTYPE_VSTRUCT;
#define END_DEFINE_VSTRUCT(name)\
	template<unsigned x>       auto&  get()       { return get(Constant<x>{}); }\
	template<unsigned x> const auto& cget() const { return get(Constant<x>{}); }\
	template<unsigned x> const auto&  get() const { return get(Constant<x>{}); }\
	template<unsigned x> static Str get_name() { return get_name(Constant<x>{}); }\
	static constexpr unsigned kCounterEnd = __COUNTER__;\
	static constexpr unsigned num_member() { return kCounterEnd - kCounterBegin; }\
	static constexpr unsigned bits() {\
		return detail::vstruct_bits<name>(::std::make_integer_sequence<unsigned, num_member()>());\
	}\
	auto Packed() const {\
		return verilog::detail::vstruct_Packed<bits()>(\
			*this,\
			::std::make_integer_sequence<unsigned, num_member()>()\
		);\
	}\
};
#ifndef VTYPE
#  define VTYPE(...) __VA_ARGS__
#endif
#define _VSTRUCT_MEMBER(tname, name, cnt) \
	      auto&  get(Constant<cnt-kCounterBegin>)       { return name; }\
	const auto& cget(Constant<cnt-kCounterBegin>) const { return name; }\
	const auto&  get(Constant<cnt-kCounterBegin>) const { return name; }\
	static Str get_name(Constant<cnt-kCounterBegin>) { return Str(#name); }\
	tname name;
#define VSTRUCT_MEMBER(tname, name) _VSTRUCT_MEMBER(VTYPE(tname), name, __COUNTER__)
