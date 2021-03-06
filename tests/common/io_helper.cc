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
#include <sstream>

namespace libmphoto {

// Returns the bytes of a file read.
std::string GetBytesFromFile(const std::string &file_name) {
  std::ifstream file(file_name, std::ifstream::in | std::ifstream::binary);
  std::ostringstream ostream;
  ostream << file.rdbuf();
  return std::string(ostream.str());
}

}  // namespace libmphoto
