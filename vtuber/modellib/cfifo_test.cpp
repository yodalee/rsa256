#include "modellib/cmodule.h"
#include "modellib/cfifo.h"
#include <gtest/gtest.h>
#include <iostream>
using namespace verilog;
using namespace std;

TEST(TestFifo, Test1) {
	vrmaster<int> fout;
	cfifo<int> f(2);
	vrslave<int> fin;
	fout(f.in);
	fin(f.out);
	vector<int> tmp;

	fin->write_func = [&](int& t) {
		if (tmp.size() >= 2) {
			return false;
		}
		tmp.push_back(t);
		return true;
	};

	EXPECT_TRUE(fout->write(111));
	f.ClockComb0();
	f.ClockSeq0();
	EXPECT_EQ(tmp.size(), 0); // write is not reflected this cycle (must wait for 1 cycle)

	EXPECT_TRUE(fout->write(222));
	f.ClockComb0();
	f.ClockSeq0();
	EXPECT_EQ(tmp.size(), 1);

	EXPECT_TRUE(fout->write(333));
	f.ClockComb0();
	f.ClockSeq0();
	EXPECT_EQ(tmp.size(), 2);

	EXPECT_TRUE(fout->write(444));
	f.ClockComb0();
	f.ClockSeq0();
	ASSERT_EQ(tmp.size(), 2);
	EXPECT_EQ(tmp[0], 111);
	EXPECT_EQ(tmp[1], 222);

	tmp.resize(0); // free the output port
	f.ClockComb0();
	EXPECT_FALSE(fout->write(555)); // but still cannot write (must wait for 1 cycle)
	f.ClockSeq0();
	ASSERT_EQ(tmp.size(), 1); // pending data in the fifo
	EXPECT_EQ(tmp[0], 333);

	EXPECT_TRUE(fout->write(555));
	f.ClockComb0();
	f.ClockSeq0();
	ASSERT_EQ(tmp.size(), 2);
	EXPECT_EQ(tmp[0], 333);
	EXPECT_EQ(tmp[1], 444);

}
