#include "abstract_random.h"
#include "abstract_random_yaml.h"
#include "testbench_config.h"
#include <glog/logging.h>
#include <iostream>
#include <yaml-cpp/yaml.h>
using namespace std;

void operator>>(const YAML::Node &node, FileInfo &info) {
  string typ = node["type"].as<::std::string>();
  if (typ == "binary") {
    info.type = FileType::kBinary;
  } else if (typ == "hexstr") {
    info.type = FileType::kHexString;
  } else {
    throw YAML::ParserException(node["type"].Mark(), "Invalid File Type");
  }
  info.path = node["path"].as<::std::string>();
}

void operator>>(const YAML::Node &node, TestbenchConfig &config) {
  config.dump = node["dump"].as<bool>();
  // parse input, output random pattern
  if (const YAML::Node &pattern_node = node["input_pattern"]) {
    config.input_pattern =
        unique_ptr<BoolPattern>(random_factory::FromYamlNode(pattern_node));
  } else {
    config.input_pattern = {};
  }
  if (const YAML::Node &pattern_node = node["output_pattern"]) {
    config.output_pattern =
        unique_ptr<BoolPattern>(random_factory::FromYamlNode(pattern_node));
  } else {
    config.output_pattern = {};
  }

  // parse input/output files
  FileInfo info;
  const YAML::Node &in_node = node["inputs"];
  for (YAML::const_iterator it = in_node.begin(); it != in_node.end(); ++it) {
    *it >> info;
    config.input_files.push_back(info);
  }
  const YAML::Node &out_node = node["outputs"];
  for (YAML::const_iterator it = out_node.begin(); it != out_node.end(); ++it) {
    *it >> info;
    config.output_files.push_back(info);
  }
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
