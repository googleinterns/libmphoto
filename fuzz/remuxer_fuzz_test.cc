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

#include <stddef.h>
#include <stdint.h>

#include <string>

#include "libmphoto/remuxer/remuxer.h"

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
  // Input less than 1mb
  if (size > 1000000) {
    return 0;
  }

  libmphoto::Remuxer remuxer;

  std::string media_stream(reinterpret_cast<const char*>(data), size);

  bool still_set = remuxer.SetStill(media_stream).ok();
  bool video_set = remuxer.SetVideo(media_stream).ok();

  if (!still_set || !video_set) {
    return 0;
  }

  std::string motion_photo;
  if (!remuxer.Finalize(&motion_photo).ok()) {
    return 0;
  }
  
  return 0;
}
