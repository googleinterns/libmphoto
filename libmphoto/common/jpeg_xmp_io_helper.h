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

#ifndef LIBMPHOTO_COMMON_JPEG_XMP_IO_HELPER_H_
#define LIBMPHOTO_COMMON_JPEG_XMP_IO_HELPER_H_

#include <memory>
#include <string>

#include "absl/status/status.h"
#include "absl/strings/string_view.h"
#include "libxml/tree.h"
#include "libmphoto/common/xmp_io_helper.h"
#include "libmphoto/common/libxml_deleter.h"

namespace libmphoto {

// This class provides enables reading/writing xmp metadata for jpeg images.
class JpegXmpIOHelper : public IXmpIOHelper {
 public:
  // Finds and parses out the xmp metadata returning the xml doc. Returns
  // nullptr if not not found.
  virtual std::unique_ptr<xmlDoc, LibXmlDeleter> GetXmp(
      const std::string &motion_photo);

  // Replaces the xmp metadata with the provided metadata.
  virtual absl::Status SetXmp(const xmlDoc &xml_doc,
                              const std::string &motion_photo,
                              std::string *updated_motion_photo);
};

}  // namespace libmphoto

#endif  // LIBMPHOTO_COMMON_JPEG_XMP_IO_HELPER_H_
