#pragma once
#include <functional>
#include "modellib/cport.h"

namespace verilog {

template<typename T>
struct cvalidready_if {
	std::function<bool(T&)> write_func;
	bool write(const T& t) { T tt = t; return write_func(tt); }
	bool write(T& t) { return write_func(t); }
};

template<typename T> using vrmaster = MasterPort<cvalidready_if<T>>;
template<typename T> using vrmaster_ex = MasterExport<cvalidready_if<T>>;
template<typename T> using vrslave = SlavePort<cvalidready_if<T>>;
template<typename T> using vrslave_ex = SlaveExport<cvalidready_if<T>>;

} // namespace verilog
