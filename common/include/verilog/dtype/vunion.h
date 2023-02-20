#pragma once
// Direct include
// C system headers
// C++ standard library headers
// Other libraries' .h files.
// Your project's .h files.
#include "namedtuple/namedtuple.h"
#include "verilog/dtype_base.h"

#define MAKE_VUNION(...) MAKE_NAMEDTUPLE(__VA_ARGS__) TAG_AS_VUNION
