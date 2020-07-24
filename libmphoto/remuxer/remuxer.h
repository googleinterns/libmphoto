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
#include "libmphoto/common/xmp_io/xmp_io_helper.h"

namespace libmphoto {

// This class provides functionality for combining encoded media
// streams to produce motion photos. SetStill and SetVideo must be called before
// Finalize.
class Remuxer {
 public:
  Remuxer();

  // Sets the still portion of the motion photo.
  absl::Status SetStill(const absl::string_view still);

  // Sets the video portion of the motion photo.
  absl::Status SetVideo(const absl::string_view video);

  // Produces a motion photo based on provided media streams.
  absl::Status Finalize(std::string *motion_photo);

 private:
  std::string still_;
  std::string video_;
  std::unique_ptr<IXmpIOHelper> xmp_io_helper_;

  absl::Status UpdateXmpMotionPhoto(xmlXPathContext *xpath_context,
                                    xmlDoc *xml_doc);
  absl::Status UpdateXmpMicrovideo(xmlXPathContext *xpath_context,
                                   xmlDoc *xml_doc);
};

}  // namespace libmphoto

#endif  // LIBMPHOTO_REMUXER_REMUXER_H_
