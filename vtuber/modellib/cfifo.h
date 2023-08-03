#pragma once
#include <boost/circular_buffer.hpp>
#include "modellib/cmodule.h"
#include "modellib/cport.h"
#include "modellib/cvalidready.h"

namespace verilog {

template<typename T>
class cfifo : public Module<cfifo<T>> {
	mutable boost::circular_buffer<T> buf_;
	// these registers are delayed by 1 cycle
	unsigned num_can_read_, num_can_write_;
	void UpdateNum() {
		num_can_read_ = buf_.size();
		num_can_write_ = buf_.capacity() - buf_.size();
	}
	bool write(T& t) {
		if (full()) {
			return false;
		}
		--num_can_write_;
		buf_.push_back(std::move(t));
		return true;
	}
public:
	vrslave<T> in;
	vrmaster<T> out;
	cfifo(unsigned s = 0) {
		set_capacity(s);
		in->write_func = [this](T& t){ return write(t); };
	}
	void set_capacity(unsigned s) {
		buf_.set_capacity(s);
		UpdateNum();
	}
	bool empty() { return num_can_read_ == 0; }
	bool full() { return num_can_write_ == 0; }
	void ClockComb0() {
		if (empty()) {
			return;
		}
		const bool write_ok = out->write(buf_.front());
		if (not write_ok) {
			return;
		}
		--num_can_read_;
		buf_.pop_front();
	}
	void ClockSeq0() {
		UpdateNum();
	}
};

} // namespace verilog
