#include "testbench_config.h"
#include "testbench_config_yaml.h"
#include "yaml-cpp/yaml.h"
#include <glog/logging.h>
#include <gtest/gtest.h>
#include <memory>
#include <string>
using namespace std;

const string kDump[]{
    R"(dump: true)",
    R"(dump: false)",
};

using namespace testbench_config;
TEST(TestParser, ReadDump) {
  unique_ptr<TestbenchConfig> dump_true(FromYamlNode(YAML::Load(kDump[0])));
  unique_ptr<TestbenchConfig> dump_false(FromYamlNode(YAML::Load(kDump[1])));
  ASSERT_TRUE(dump_true->dump);
  ASSERT_FALSE(dump_false->dump);
}
