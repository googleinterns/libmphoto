// Copyright 2020 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

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
