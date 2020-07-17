// Copyright 2020 Google LLC
// Author: pinheirojamie

#ifndef TESTS_COMMON_IO_HELPER_H_
#define TESTS_COMMON_IO_HELPER_H_

#include <string>

namespace libmphoto {

// Returns the bytes of a file read.
std::string GetBytesFromFile(const std::string &file_name);

}  // namespace libmphoto

#endif  // TESTS_COMMON_IO_HELPER_H_
