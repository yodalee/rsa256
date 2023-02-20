#pragma once
// Direct include
// C system headers
// C++ standard library headers
#include <type_traits>
#include <utility>
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
[[maybe_unused]]
static const char *dtype_names[DTYPE_MAX] = {
	"vint",
	"varray",
	"vstruct",
	"vunion"
};
#define TAG_AS_VINT static constexpr unsigned dtype_tag = DTYPE_VINT;
#define TAG_AS_VARRAY static constexpr unsigned dtype_tag = DTYPE_VARRAY;
#define TAG_AS_VSTRUCT static constexpr unsigned dtype_tag = DTYPE_VSTRUCT;
#define TAG_AS_VUNION static constexpr unsigned dtype_tag = DTYPE_VUNION;
template <bool is_signed, unsigned num_bit> struct vint;
template <class T, unsigned... dims> struct varray;

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

// TODO: move this to independent headers
///////////////////
// analog to verilog $bits
// an abstraction layer to calculate the number of bits of a datatype
// also put forward declaration here
///////////////////
namespace detail {
template<typename T, unsigned...i>
constexpr unsigned vstruct_bits(::std::integer_sequence<unsigned, i...>);
// template<typename T, unsigned...i>
// constexpr unsigned vunion_bits(::std::integer_sequence<unsigned, i...>);
} // namespace detail
template<typename T_> constexpr unsigned bits() {
	typedef ::std::remove_reference_t<T_> T;
	static_assert(is_dtype_v<T>, "T must be a verilog type");
	if constexpr (is_vint_v<T>) {
		return T::num_bit;
	} else if constexpr (is_varray_v<T>) {
		return bits<typename T::dtype>() * T::asize;
	} else if constexpr (is_vstruct_v<T>) {
		return detail::vstruct_bits<T>(::std::make_integer_sequence<unsigned, T::num_members>());
//	} else if constexpr (is_vunion_v<T>) {
//		return detail::vunion_bits<T>(::std::make_integer_sequence<unsigned, T::num_member>());
	}
}
template<typename T> inline constexpr unsigned bits_v = bits<T>();

///////////////////
// analog to verilog {>>{}} (packing)
// an abstraction layer to pack a datatype
// also put forward declaration here
///////////////////
namespace detail {
template<typename T>
auto vint_packed(const T &src);
template<typename T, unsigned ...i>
auto varray_packed(const T &src, ::std::integer_sequence<unsigned, i...>);
template<typename T, unsigned ...i>
auto vstruct_packed(const T &src, ::std::integer_sequence<unsigned, i...>);
} // namespace detail
template<typename T>
vint<false, bits<T>()> packed(const T& t) {
	static_assert(is_dtype_v<T>, "T must be a verilog type");
	if constexpr (is_vint_v<T>) {
		return detail::vint_packed<T>(t);
	} else if constexpr (is_varray_v<T>) {
		return detail::varray_packed<T>(t, ::std::make_integer_sequence<unsigned, T::asize>());
	} else if constexpr (is_vstruct_v<T>) {
		return detail::vstruct_packed<T>(t, ::std::make_integer_sequence<unsigned, T::num_members>());
//	} else if constexpr (is_vunion_v<T>) {
//		return vint<false, 0u>();
	}
}

} // namespace verilog
