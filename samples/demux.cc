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
#include "samples/macros.h"

int main(int argc, char *argv[]) {
  if (argc != 2) {
    std::cout << "Usage: demux <image_file>" << std::endl;
    return -1;
  }

  // Read entire file into memory.
  std::ifstream file(argv[1], std::ifstream::in | std::ifstream::binary);
  std::ostringstream ostream;
  ostream << file.rdbuf();
  std::string file_bytes(ostream.str());

  // Initialize demuxer.
  libmphoto::Demuxer demuxer;
  TERMINATE_IF_ERROR(demuxer.Init(file_bytes))

  // Get image info.
  libmphoto::ImageInfo image_info;
  TERMINATE_IF_ERROR(demuxer.GetInfo(&image_info))

  // Print out image information.
  std::cout << image_info.toString() << std::endl;

  // Get still image file.
  std::string still;
  TERMINATE_IF_ERROR(demuxer.GetStill(&still));
  std::ofstream out_still("still.jpeg");
  out_still << still;
  out_still.close();

  // Get video file.
  std::string video;
  TERMINATE_IF_ERROR(demuxer.GetVideo(&video));
  std::ofstream out_video("video.mp4");
  out_video << video;
  out_video.close();
}
