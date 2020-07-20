/*
 * Copyright 2020 Google LLC
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef LIBMPHOTO_REMUXER_REMUXER_H_
#define LIBMPHOTO_REMUXER_REMUXER_H_

#include <memory>
#include <string>

#include "absl/status/status.h"
#include "absl/strings/string_view.h"

namespace libmphoto {

// This class provides functionality for replacing/combining encoded media
// streams in motion photos. Init must first be called before any other
// class functions can be called.
class Remuxer {
 public:
  Remuxer();

  // Initializes the demuxer with an optional existing motion_photo.
  absl::Status Init(const std::string *motion_photo);

  // Replaces the still portion of a motion photo with a new encoded stream.
  absl::Status SetStill(const absl::string_view still);

  // Replaces the video portion of a motion photo with a new encoded stream.
  absl::Status SetVideo(const absl::string_view video);

  // Produces the bytes of the new motion photo with updated streams.
  absl::Status GetMotionPhoto(std::string *motion_photo);
};

}  // namespace libmphoto

#endif  // LIBMPHOTO_REMUXER_REMUXER_H_
