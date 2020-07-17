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

  // Get still image file
  std::string still;
  demuxer.GetStill(&still);
  std::ofstream out_still("still.jpg");
  out_still << still;
  out_still.close();

  // Get video file
  std::string video;
  demuxer.GetVideo(&video);
  std::ofstream out_video("video.mp4");
  out_video << video;
  out_video.close();
}
