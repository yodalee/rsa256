#include "modellib/cport.h"
#include <gtest/gtest.h>
using namespace verilog;

struct Interface {
	int value;
};

TEST(TestPort, Plain) {
	MasterPort<Interface> p_m;
	MasterExport<Interface> p_m_e;
	p_m(p_m_e);
	MasterExport<Interface> p_m_e_e(p_m_e);
	SlavePort<Interface> p_s;
	SlaveExport<Interface> p_s_e(p_s);
	SlaveExport<Interface> p_s_e_e;
	p_s_e(p_s_e_e);
	p_m(p_s);

	p_m->value = 12345;
	EXPECT_EQ(p_s->value, 12345);
}

TEST(TestPort, PlainReversed) {
	MasterPort<Interface> p_m;
	MasterExport<Interface> p_m_e;
	p_m_e(p_m);
	MasterExport<Interface> p_m_e_e(p_m_e);
	SlavePort<Interface> p_s;
	SlaveExport<Interface> p_s_e(p_s);
	SlaveExport<Interface> p_s_e_e;
	p_s_e_e(p_s_e);
	p_s(p_m);

	p_m->value = 12345;
	EXPECT_EQ(p_s->value, 12345);
}
