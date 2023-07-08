#pragma once

namespace YAML { class Node; }
class BoolPattern;

namespace random_factory {

BoolPattern* FromYamlNode(const YAML::Node& node);

}
