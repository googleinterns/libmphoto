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

#include "libmphoto/demuxer/demuxer.h"
#include "libmphoto/demuxer/image_info.h"

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
  libmphoto::Demuxer demuxer;

  std::string motion_photo(reinterpret_cast<const char*>(data), size);

  if (!demuxer.Init(motion_photo).ok()) {
    return 0;
  }

  std::string still;
  if (!demuxer.GetStill(&still).ok()) {
    return 0;
  }

  std::string video;
  if (!demuxer.GetVideo(&video).ok()) {
    return 0;
  }

  libmphoto::ImageInfo image_info;
  if (!demuxer.GetInfo(&image_info).ok()) {
    return 0;
  }

  return 0;
}
