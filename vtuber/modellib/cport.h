#pragma once

namespace verilog {

template<typename T> struct MasterPort;
template<typename T> struct MasterExport;
template<typename T> struct SlavePort;
template<typename T> struct SlaveExport;

template<typename T>
struct MasterPort {
	T* t = nullptr;
	T* operator->() { return t; }
	MasterPort() {}
	// to Master
	void operator()(MasterExport<T>& rhs) { rhs.t = &t; }
	// to Slave
	MasterPort(SlavePort<T>& rhs) { this->operator()(rhs); }
	MasterPort(SlaveExport<T>& rhs) { this->operator()(rhs); }
	void operator()(SlavePort<T>& rhs) { t = &rhs.t; }
	void operator()(SlaveExport<T>& rhs) { t = rhs.t; }
};

template<typename T>
struct MasterExport {
	T** t = nullptr;
	MasterExport() {}
	T* operator->() { return *t; }
	// to Master
	MasterExport(MasterPort<T>& rhs) { this->operator()(rhs); }
	MasterExport(MasterExport& rhs) { this->operator()(rhs); }
	void operator()(MasterPort<T>& rhs) { t = &rhs.t; }
	void operator()(MasterExport& rhs) { if (t == nullptr) { t = rhs.t; } else { rhs.t = t; } }
	// to Slave
	MasterExport(SlavePort<T>& rhs) { this->operator()(rhs); }
	MasterExport(SlaveExport<T>& rhs) { this->operator()(rhs); }
	void operator()(SlavePort<T>& rhs) { *t = &rhs.t; }
	void operator()(SlaveExport<T>& rhs) { *t = rhs.t; }
};

template<typename T>
struct SlavePort {
	T t;
	T* operator->() { return &t; }
	SlavePort() {}
	// to Master
	SlavePort(MasterPort<T>& rhs) { this->operator()(rhs); }
	SlavePort(MasterExport<T>& rhs) { this->operator()(rhs); }
	void operator()(MasterPort<T>& rhs) { rhs(*this); }
	void operator()(MasterExport<T>& rhs) { rhs(*this); }
	// to Slave
	void operator()(SlaveExport<T>& rhs) { rhs.t = &t; }
};

template<typename T>
struct SlaveExport {
	T* t = nullptr;
	SlaveExport() {}
	// to Master
	SlaveExport(MasterPort<T>& rhs) { this->operator()(rhs); }
	SlaveExport(MasterExport<T>& rhs) { this->operator()(rhs); }
	void operator()(MasterPort<T>& rhs) { rhs(*this); }
	void operator()(MasterExport<T>& rhs) { rhs(*this); }
	// to Slave
	SlaveExport(SlavePort<T>& rhs) { this->operator()(rhs); }
	SlaveExport(SlaveExport& rhs) { this->operator()(rhs); }
	void operator()(SlavePort<T>& rhs) { t = &rhs.t; }
	void operator()(SlaveExport& rhs) { if (t == nullptr) { t = rhs.t; } else { rhs.t = t; } }
};

} // namespace verilog
