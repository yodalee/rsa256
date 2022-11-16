#pragma once
#include <algorithm>
#include <memory>

struct BoolPattern {
	virtual bool operator()() = 0;
	virtual void seed(unsigned) = 0;
	virtual ~BoolPattern() {}
};

struct RandomBool: public BoolPattern {
	RandomBool(float possibility1):
		gen_(1),
		dist_(possibility1) {}
	bool operator()() override {
		return dist_(gen_);
	}
	void seed(unsigned s) override {
		gen_.seed(s);
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
		current_ = s % ::std::max(ratio0_, ratio1_);
	}
private:
	const unsigned ratio1_, ratio0_;
	unsigned current_;
};

namespace random_factory {

// Note: you shall store them by unique_ptr
BoolPattern* AlwaysOne() {
	return new RandomBool(1, 0);
}

BoolPattern* AlwaysZero() {
	return new RepeatBool(0, 1);
}

BoolPattern* OneEvery(unsigned x) {
	x = std::min(1u, x);
	return new RepeatBool(1, x-1);
}

BoolPattern* Bernoulli(float p) {
	return RandomBool(p);
}

}
