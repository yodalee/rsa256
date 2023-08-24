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

const string kFileInfo[]{
    R"(dump: false
inputs:
  - type: binary
    path: a/b/c)",
    R"(dump: false
outputs:
  - type: hexstr
    path: /a/b/c)",
    R"(dump: false
inputs:
  - type: binary
    path: p1
  - type: hexstr
    path: p2)"};

using namespace testbench_config;
TEST(TestParser, ReadDump) {
  unique_ptr<TestbenchConfig> dump_true(FromYamlNode(YAML::Load(kDump[0])));
  unique_ptr<TestbenchConfig> dump_false(FromYamlNode(YAML::Load(kDump[1])));
  ASSERT_TRUE(dump_true->dump);
  ASSERT_FALSE(dump_false->dump);
  ASSERT_FALSE(dump_true->input_pattern);
  ASSERT_FALSE(dump_true->output_pattern);
  ASSERT_FALSE(dump_false->input_pattern);
  ASSERT_FALSE(dump_false->output_pattern);
}

TEST(TestParser, ReadFileInfo) {
  unique_ptr<TestbenchConfig> inputs(FromYamlNode(YAML::Load(kFileInfo[0])));
  unique_ptr<TestbenchConfig> outputs(FromYamlNode(YAML::Load(kFileInfo[1])));
  ASSERT_EQ(inputs->input_files.size(), 1);
  ASSERT_EQ(inputs->output_files.size(), 0);
  ASSERT_EQ(inputs->input_files[0].type, FileType::kBinary);
  ASSERT_EQ(inputs->input_files[0].path, "a/b/c");
  ASSERT_EQ(outputs->input_files.size(), 0);
  ASSERT_EQ(outputs->output_files.size(), 1);
  ASSERT_EQ(outputs->output_files[0].type, FileType::kHexString);
  ASSERT_EQ(outputs->output_files[0].path, "/a/b/c");

  unique_ptr<TestbenchConfig> multiple_inputs(
      FromYamlNode(YAML::Load(kFileInfo[2])));
  ASSERT_EQ(multiple_inputs->input_files.size(), 2);
  ASSERT_EQ(multiple_inputs->output_files.size(), 0);
  ASSERT_EQ(multiple_inputs->input_files[0].type, FileType::kBinary);
  ASSERT_EQ(multiple_inputs->input_files[0].path, "p1");
  ASSERT_EQ(multiple_inputs->input_files[1].type, FileType::kHexString);
  ASSERT_EQ(multiple_inputs->input_files[1].path, "p2");
}