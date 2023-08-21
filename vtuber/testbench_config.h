#pragma once

#include "abstract_random.h"

#include <memory>
#include <vector>

struct TestbenchConfig {
  bool dump;
  std::unique_ptr<BoolPattern> input_pattern;
  std::unique_ptr<BoolPattern> output_pattern;
};