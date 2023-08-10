#pragma once

#include <memory>

namespace YAML {
class Node;
}

namespace testbench_config {

::std::unique_ptr<TestbenchConfig> FromYamlNode(const YAML::Node &node);

} // namespace testbench_config
