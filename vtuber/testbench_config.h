#pragma once

#include "abstract_random.h"

#include <memory>
#include <vector>

enum class FileType { kBinary, kHexString };

struct FileInfo {
  FileType type;
  ::std::string path;
};

struct TestbenchConfig {
  bool dump;
  ::std::unique_ptr<BoolPattern> input_pattern;
  ::std::unique_ptr<BoolPattern> output_pattern;
  ::std::vector<FileInfo> input_files;
  ::std::vector<FileInfo> output_files;
};