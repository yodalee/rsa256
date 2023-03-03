#pragma once
// Direct include
// C system headers
// C++ standard library headers
#include <ostream>
#include <utility>
// Other libraries' .h files.
// Your project's .h files.
#include "namedtuple/namedtuple.h"
#include "verilog/dtype_base.h"
#include "verilog/operation/print_indent.h"

#define MAKE_VSTRUCT(...) MAKE_NAMEDTUPLE(__VA_ARGS__) TAG_AS_VSTRUCT
#define VSTRUCT_HAS_PROCESS(cls) \
void print_with_helper(verilog::detail::ost_indent_helper &helper) const {\
	helper.force_skip_next_new_line();\
	verilog::detail::vstruct_print_with_helper_expander(\
		helper, *this,\
		::std::make_integer_sequence<unsigned, num_members>()\
	);\
}\
friend ::std::ostream& operator<<(::std::ostream& ost, const cls& rhs) {\
	verilog::detail::ost_indent_helper helper(ost);\
	rhs.print_with_helper(helper);\
	return ost;\
}
