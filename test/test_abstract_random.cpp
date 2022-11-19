#include "abstract_random.h"
#include <gtest/gtest.h>
#include <array>
#include <memory>
#include <cmath>
#include <string>
#include <tuple>
using namespace std;
static const unsigned seeds[]{0u, 123456u, ~0u};

TEST(TestFactory, AlwaysOne) {
	unique_ptr<BoolPattern> ptr{random_factory::AlwaysOne()};
	for (unsigned s: seeds) {
		ptr->seed(s);
		constexpr int kTotalCount = 100;
		int count = 0;
		for (int i = 0; i < kTotalCount; ++i) {
			if (ptr->operator()()) {
				++count;
			}
		}
		EXPECT_EQ(count, kTotalCount);
	}
}

TEST(TestFactory, AlwaysZero) {
	unique_ptr<BoolPattern> ptr{random_factory::AlwaysZero()};
	for (unsigned s: seeds) {
		ptr->seed(s);
		constexpr int kTotalCount = 100;
		int count = 0;
		for (int i = 0; i < kTotalCount; ++i) {
			if (ptr->operator()()) {
				++count;
			}
		}
		EXPECT_EQ(count, 0);
	}
}

TEST(TestFactory, OneEvery) {
	for (int every_n = 1; every_n < 10; ++every_n) {
		unique_ptr<BoolPattern> ptr{random_factory::OneEvery(every_n)};
		for (unsigned s: seeds) {
			ptr->seed(s);
			int last_appear = -1;
			for (int i = 0; i < 100; ++i) {
				if (ptr->operator()()) {
					if (last_appear != -1) {
						EXPECT_EQ(last_appear+every_n, i);
					}
					last_appear = i;
				}
			}
		}
	}
}

TEST(TestFactory, BernoulliZeroOne) {
	// prob is 0 or 1
	for (int prob = 0; prob < 2; ++prob) {
		unique_ptr<BoolPattern> ptr{random_factory::Bernoulli(float(prob))};
		for (unsigned s: seeds) {
			ptr->seed(s);
			constexpr int kTotalCount = 1000;
			int count = 0;
			for (int i = 0; i < kTotalCount; ++i) {
				if (ptr->operator()()) {
					++count;
				}
			}
			EXPECT_EQ(count, kTotalCount*prob);
		}
	}
}

TEST(TestFactory, BernoulliNormal) {
	// 0.1, 0.2, ..., 0.9
	for (float prob = 0.1f; prob < 0.91f; prob += 0.1f) {
		unique_ptr<BoolPattern> ptr{random_factory::Bernoulli(float(prob))};
		for (unsigned s: seeds) {
			ptr->seed(s);
			constexpr int kTotalCount = 1000;
			int count = 0;
			for (int i = 0; i < kTotalCount; ++i) {
				if (ptr->operator()()) {
					++count;
				}
			}
			// const float stdev = 0.5 / std::sqrt(kTotalCount);
			const float max_error = 0.5 * std::sqrt(kTotalCount); // stdev * kTotalCount
			EXPECT_LE(abs(float(count)/kTotalCount-prob), 3.5 * max_error);
		}
	}
}

TEST(TestFactory, RepeatBool) {
	// 1:9, 2:8, 3:7, ..., 9:1
	for (int prob = 1; prob < 10; ++prob) {
		unique_ptr<BoolPattern> ptr{new RepeatBool(prob, 10-prob)};
		for (unsigned s: seeds) {
			ptr->seed(s);
			constexpr int kTotalCount = 1000;
			int count = 0;
			for (int i = 0; i < kTotalCount; ++i) {
				if (ptr->operator()()) {
					++count;
				}
			}
			EXPECT_LE(abs(float(count)/kTotalCount-prob*0.1f), 0.005f);
		}
	}
}
