#pragma once
#include <algorithm>
#include <memory>
#include <random>

struct BoolPattern {
	virtual bool operator()() = 0;
	virtual void seed(unsigned) = 0;
	virtual ~BoolPattern() {}
};

struct RandomBool: public BoolPattern {
	RandomBool(double possibility1):
		gen_(1),
		dist_(possibility1) {}
	bool operator()() override {
		return dist_(gen_);
	}
	void seed(unsigned s) override {
		gen_.seed(s);
	}
	void get_possibility(float& possibility) const {
		possibility = dist_.p();
	}
private:
	::std::default_random_engine gen_;
	::std::bernoulli_distribution dist_;
};

struct RepeatBool: public BoolPattern {
	RepeatBool(unsigned ratio1, unsigned ratio0):
		ratio1_(ratio1),
		ratio0_(ratio0),
		current_(0) {}
	bool operator()() override {
		if (current_ >= ratio0_) {
			current_ -= ratio0_;
			return true;
		}
		current_ += ratio1_;
		return false;
	}
	void seed(unsigned s) override {
		current_ = s % ::std::max(ratio0_, ratio1_+1u);
	}
	void get_ratio(unsigned& ratio1, unsigned& ratio0) const {
		ratio1 = ratio1_;
		ratio0 = ratio0_;
	}
private:
	const unsigned ratio1_, ratio0_;
	unsigned current_;
};

namespace random_factory {

// Note: you shall store them by unique_ptr
static inline BoolPattern* AlwaysOne() {
	return new RepeatBool(1, 0);
}

static inline BoolPattern* AlwaysZero() {
	return new RepeatBool(0, 1);
}

static inline BoolPattern* OneEvery(unsigned x) {
	x = std::max(1u, x);
	return new RepeatBool(1, x-1);
}

static inline BoolPattern* Bernoulli(float p) {
	return new RandomBool(p);
}

}
