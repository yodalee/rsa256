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

} // namespace verilog
