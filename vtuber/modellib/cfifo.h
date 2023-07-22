#pragma once
#include <boost/circular_buffer.hpp>
#include <glog/logging.h>
#include "modellib/cmodule.h"

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
public:
	cfifo() { UpdateNum(); }
	cfifo(unsigned s) : buf_(s) {
		UpdateNum();
	}
	void resize(unsigned s) {
		buf_.resize(s);
		UpdateNum();
	}
	bool empty() { return num_can_read_ == 0; }
	bool full() { return num_can_write_ == 0; }
	T read() {
		DCHECK(not empty());
		--num_can_read_;
		T ret = std::move(buf_.front());
		buf_.pop_front();
		return ret;
	}
	void write(const T& t) {
		DCHECK(not full());
		--num_can_write_;
		buf_.push_back(t);
	}
	void write(T&& t) {
		DCHECK(not full());
		--num_can_write_;
		buf_.push_back(std::move(t));
	}
	void ClockSeq0() {
		UpdateNum();
	}
};

template<typename T> using cfifo_in = cfifo<T>*;
template<typename T> using cfifo_out = cfifo<T>*;
template<typename T> using cfifo_in_export = cfifo<T>**;
template<typename T> using cfifo_out_export = cfifo<T>**;

} // namespace verilog
