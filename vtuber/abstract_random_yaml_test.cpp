#include <string>
#include <memory>
#include <gtest/gtest.h>
#include <glog/logging.h>
#include "abstract_random.h"
#include "abstract_random_yaml.h"
#include "yaml-cpp/yaml.h"
using namespace std;
const string patterns[]{
// pattern 1
R"(RandomBool:
  possibility1: 0.7)",
// pattern 2
R"(RandomBool:)",
// pattern 3
R"(RepeatBool:
  ratio0: 3
  ratio1: 7)",
// pattern 4
R"(RepeatBool:)",
};

void PlayRandom(
	BoolPattern& gen, const string& name,
	unsigned total, unsigned accepted_lo, unsigned accepted_hi
) {
	unsigned count = 0;
	for (int i = 0; i < total; ++i) {
		if (gen()) {
			++count;
		}
	}
	CHECK_GE(count, accepted_lo) << name;
	CHECK_LE(count, accepted_hi) << name;
}

using namespace random_factory;
TEST(TestFactory, FromYamlRamdomBool) {
	unique_ptr<BoolPattern> gen0(FromYamlNode(YAML::Load(patterns[0])));
	unique_ptr<BoolPattern> gen1(FromYamlNode(YAML::Load(patterns[1])));
	unique_ptr<BoolPattern> gen2(FromYamlNode(YAML::Load(patterns[2])));
	unique_ptr<BoolPattern> gen3(FromYamlNode(YAML::Load(patterns[3])));
	ASSERT_TRUE(gen0);
	ASSERT_TRUE(gen1);
	ASSERT_TRUE(gen2);
	ASSERT_TRUE(gen3);
	PlayRandom(*gen0, "gen0", 1000, 650, 750);
	PlayRandom(*gen1, "gen1", 1000, 450, 550);
	PlayRandom(*gen2, "gen2", 1000, 650, 750);
	PlayRandom(*gen3, "gen3", 1000, 450, 550);
}
