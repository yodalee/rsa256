#pragma once
// Direct include
// C system headers
// C++ standard library headers
#include <type_traits>
// Other libraries' .h files.
// Your project's .h files.

namespace verilog {

enum Dtype: unsigned {
	DTYPE_VINT = 0u,
	DTYPE_VARRAY,
	DTYPE_VSTRUCT,
	DTYPE_VUNION,
	DTYPE_MAX,
};
static const char *dtype_names[DTYPE_MAX] = {
	"vint",
	"varray",
	"vstruct",
	"vunion"
};
typedef ::std::integral_constant<unsigned, DTYPE_VINT> vint_tag_t;
typedef ::std::integral_constant<unsigned, DTYPE_VARRAY> varray_tag_t;
typedef ::std::integral_constant<unsigned, DTYPE_VSTRUCT> vstruct_tag_t;
typedef ::std::integral_constant<unsigned, DTYPE_VUNION> vunion_tag_t;

// is_dtype(_v)
template<typename T, typename = void>
struct is_dtype: ::std::false_type {};
template<typename T>
struct is_dtype<
	T,
	::std::enable_if_t<sizeof(T::dtype_tag) != 0>
>: public ::std::true_type {};
template<typename T>
inline constexpr bool is_dtype_v = is_dtype<T>::value;
// is_dtype_of_id(_v)
template<typename T, unsigned tag_id, typename = void>
struct is_dtype_of_id: ::std::false_type {};
template<typename T, unsigned tag_id>
struct is_dtype_of_id<
	T, tag_id,
	::std::enable_if_t<tag_id == T::dtype_tag>
>: public ::std::true_type {};
template<typename T, unsigned tag_idx>
inline constexpr bool is_dtype_of_id_v = is_dtype_of_id<T, tag_idx>::value;

template<typename T>
inline constexpr bool is_vint_v = is_dtype_of_id<T, DTYPE_VINT>::value;
template<typename T>
inline constexpr bool is_varray_v = is_dtype_of_id<T, DTYPE_VARRAY>::value;
template<typename T>
inline constexpr bool is_vstruct_v = is_dtype_of_id<T, DTYPE_VSTRUCT>::value;
template<typename T>
inline constexpr bool is_vunion_v = is_dtype_of_id<T, DTYPE_VUNION>::value;
///////////////////
// analog to verilog $bits
// currently it is simply an abstraction layer of T::bits()
///////////////////
template<typename T> constexpr unsigned bits() {
	static_assert(is_dtype_v<T>, "Cannot compute bits of T");
	return T::bits();
}

} // namespace verilog
