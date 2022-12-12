#pragma once
#include <iostream>

namespace verilog {

template <class T>
::std::ostream& SaveContent(::std::ostream &os, const T& rhs) {
	static_assert(::std::is_trivially_copyable_v<T>);
	os.write(reinterpret_cast<const char*>(&rhs), sizeof(T));
	return os;
};

template <class T>
::std::istream& LoadContent(::std::istream &ist, T& rhs) {
	static_assert(::std::is_trivially_copyable_v<T>);
	ist.read(reinterpret_cast<char*>(&rhs), sizeof(T));
	return ist;
};

} // namespace verilog
