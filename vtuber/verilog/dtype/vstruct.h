#pragma once
// Direct include
// C system headers
// C++ standard library headers
#include <ostream>
// Other libraries' .h files.
// Your project's .h files.
#include "namedtuple/namedtuple.h"
#include "verilog/dtype_base.h"
#include "verilog/operation/print_indent.h"

#define MAKE_VSTRUCT(...) MAKE_NAMEDTUPLE(__VA_ARGS__) TAG_AS_VSTRUCT
#define VSTRUCT_HAS_PROCESS(cls) \
friend ::std::ostream& operator<<(::std::ostream& ost, const cls& rhs) {\
	verilog::print_indent(ost, 0u, rhs);\
	return ost;\
}
