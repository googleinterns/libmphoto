// Copyright 2020 Google LLC
// Author: pinheirojamie

#include <fstream>
#include <iostream>
#include <vector>

#include "absl/status/status.h"
#include "libmphoto/demuxer/demuxer.h"
#include "libmphoto/demuxer/image_info.h"

int main(int argc, char *argv[]) {
  if (argc != 2) {
    std::cout << "Usage: demux <image_file>" << std::endl;
    return -1;
  }

  // Read entire file into memory
  std::ifstream file(argv[1], std::ifstream::in | std::ifstream::binary);
  file.seekg(0, std::ios::end);
  std::streampos length = file.tellg();
  file.seekg(0, std::ios::beg);
  std::vector<uint8_t> data(length);
  file.read((char *)data.data(), length);
  std::string file_bytes(data.begin(), data.end());

  // Initialize demuxer
  libmphoto::Demuxer demuxer;
  absl::Status status = demuxer.Init(file_bytes);
  if (!status.ok()) {
    std::cout << status << std::endl;
    return -1;
  }

  // Get image info
  libmphoto::ImageInfo image_info;
  status = demuxer.GetInfo(&image_info);
  if (!status.ok()) {
    std::cout << status << std::endl;
    return -1;
  }

  // Print out image information
  std::cout << image_info.toString() << std::endl;
}
