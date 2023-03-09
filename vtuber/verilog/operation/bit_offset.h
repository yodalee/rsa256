#pragma once
// Direct include
// C system headers
// C++ standard library headers
// Other libraries' .h files.
// Your project's .h files.
#include "verilog/dtype_base.h"
#include "verilog/operation/bits.h"

namespace verilog {

template<typename T, unsigned i>
constexpr unsigned bit_offset_first_member_at_msb() {
	static_assert(0 <= i and i < T::num_members);
	if constexpr (i == (T::num_members-1)) {
		return 0;
	} else {
		T* tp = nullptr;
		return (
			  verilog::bits<decltype(tp->template get<i+1>())>()
			+ bit_offset_first_member_at_msb<T, i+1u>()
		);
	}
}

template<typename T, unsigned i>
constexpr unsigned bit_offset_first_member_at_lsb() {
	static_assert(0 <= i and i < T::num_members);
	if constexpr (i == 0) {
		return 0u;
	} else {
		T* tp = nullptr;
		return (
			  verilog::bits<decltype(tp->template get<i-1>())>()
			+ bit_offset_first_member_at_lsb<T, i-1>()
		);
	}
}

template<typename T, unsigned i> inline constexpr unsigned
bit_offset_first_member_at_lsb_v = bit_offset_first_member_at_lsb<T, i>();
template<typename T, unsigned i> inline constexpr unsigned
bit_offset_first_member_at_msb_v = bit_offset_first_member_at_msb<T, i>();

} // namespace verilog
