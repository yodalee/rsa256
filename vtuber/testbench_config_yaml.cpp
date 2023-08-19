#include "testbench_config.h"
#include <glog/logging.h>
#include <iostream>
#include <yaml-cpp/yaml.h>
using namespace std;

void operator>>(const YAML::Node &node, TestbenchConfig &config) {
  config.dump = node["dump"].as<bool>();
}

namespace testbench_config {

unique_ptr<TestbenchConfig> FromYamlNode(const YAML::Node &node) {
  try {
    unique_ptr<TestbenchConfig> config = make_unique<TestbenchConfig>();
    node >> *config;
    return config;
  } catch (YAML::ParserException &e) {
    LOG(ERROR) << e.what();
    return unique_ptr<TestbenchConfig>{};
  }
}

} // namespace testbench_config
