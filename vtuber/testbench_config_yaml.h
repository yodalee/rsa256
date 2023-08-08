#pragma once

#include <memory>
#include <yaml-cpp/yaml.h>

namespace testbench_config {

::std::unique_ptr<TestbenchConfig> FromYamlNode(const YAML::Node &node);

} // namespace testbench_config
