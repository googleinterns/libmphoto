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

#ifndef LIBMPHOTO_COMMON_XMP_IO_XMP_IO_HELPER_H_
#define LIBMPHOTO_COMMON_XMP_IO_XMP_IO_HELPER_H_

#include <memory>
#include <string>

#include "absl/status/status.h"
#include "absl/strings/string_view.h"
#include "libxml/tree.h"
#include "libmphoto/common/xml/libxml_deleter.h"
#include "libmphoto/common/mime_type.h"

namespace libmphoto {

// This interface provides a basic contract for reading/writing xmp metadata
// to image streams. It's to be implemented for a specific format (ie. jpeg or
// heic).
class IXmpIOHelper {
 public:
  // Finds and parses out the xmp metadata returning the xml doc. Returns
  // nullptr if not not found.
  virtual std::unique_ptr<xmlDoc, LibXmlDeleter> GetXmp(
      const std::string &image) = 0;

  // Replaces the xmp metadata with the provided metadata.
  virtual absl::Status SetXmp(const xmlDoc &xml_doc, const std::string &image,
                              std::string *updated_image) = 0;

  // Gets the mime type this xmp helper is implemented for.
  virtual MimeType GetMimeType() = 0;
};

// Returns the appropriate xmp helper for a given image stream.
std::unique_ptr<IXmpIOHelper> GetXmpIOHelper(const std::string &image);

enum class MPhotoFormat { kNone = 0, kMotionPhoto, kMicrovideo };

// Returns the the MPhotoFormat for a given xmp metadata.
MPhotoFormat GetMPhotoFormat(const xmlXPathContext &xpath_context);

}  // namespace libmphoto

#endif  // LIBMPHOTO_COMMON_XMP_IO_XMP_IO_HELPER_H_
