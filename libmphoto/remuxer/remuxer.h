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

// This class provides functionality for replacing encoded media
// streams in motion photos.
class Remuxer {
 public:
  Remuxer();

  // Produces a motion photo from an existing one with the still image portion
  // replaced with a new encoded stream.
  absl::Status ReplaceStill(const absl::string_view new_still,
                            const absl::string_view current_motion_photo,
                            std::string *motion_photo);

  // Produces a motion photo from an existing one with the video portion
  // replaced with a new encoded stream.
  absl::Status ReplaceVideo(const absl::string_view new_video,
                            const absl::string_view current_motion_photo,
                            std::string *motion_photo);
};

}  // namespace libmphoto

#endif  // LIBMPHOTO_REMUXER_REMUXER_H_
