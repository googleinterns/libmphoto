// Copyright 2020 Google LLC
// Author: pinheirojamie

#include "tests/common/io_helper.h"

#include <fstream>
#include <iostream>
#include <vector>

namespace libmphoto {

// Returns the bytes of a file read.
std::string GetBytesFromFile(const std::string &file_name) {
  std::ifstream file(file_name, std::ifstream::in | std::ifstream::binary);
  file.seekg(0, std::ios::end);
  std::streampos length = file.tellg();
  file.seekg(0, std::ios::beg);
  std::vector<uint8_t> data(length);
  file.read(reinterpret_cast<char *>(data.data()), length);
  return std::string(data.begin(), data.end());
}

}  // namespace libmphoto
