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
#include <sstream>

#include "absl/status/status.h"
#include "absl/strings/numbers.h"
#include "libmphoto/remuxer/remuxer.h"
#include "samples/macros.h"

int main(int argc, char *argv[]) {
  if (argc != 4) {
    std::cout << "Usage: remux <still_file> <video_file> <still_timestamp>"
              << std::endl;
    return -1;
  }

  // Read still into memory.
  std::ifstream still_file(argv[1], std::ifstream::in | std::ifstream::binary);
  std::ostringstream still_ostream;
  still_ostream << still_file.rdbuf();
  std::string still(still_ostream.str());

  // Read video into memory.
  std::ifstream video_file(argv[2], std::ifstream::in | std::ifstream::binary);
  std::ostringstream video_ostream;
  video_ostream << video_file.rdbuf();
  std::string video(video_ostream.str());

  // Initialize remuxer.
  libmphoto::Remuxer remuxer;

  // Set media streams.
  int timestamp;
  if (!absl::SimpleAtoi(argv[3], &timestamp)) {
    std::cout << "Invalid still timestamp" << std::endl;
    return -1;
  }
  TERMINATE_IF_ERROR(remuxer.SetStill(still, timestamp));
  TERMINATE_IF_ERROR(remuxer.SetVideo(video));

  // Get new motion photo.
  std::string motion_photo;
  TERMINATE_IF_ERROR(remuxer.Finalize(&motion_photo));
  std::ofstream out_mp("motion_photo");
  out_mp << motion_photo;
  out_mp.close();
}
