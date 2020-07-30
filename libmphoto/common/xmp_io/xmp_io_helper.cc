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

#include "libmphoto/common/xmp_io/xmp_io_helper.h"

#include <vector>
#include <algorithm>

#include "absl/strings/string_view.h"
#include "libmphoto/common/xmp_io/jpeg_xmp_io_helper.h"
#include "libmphoto/common/xmp_io/heic_xmp_io_helper.h"
#include "libmphoto/common/xml/xml_utils.h"
#include "libmphoto/common/xmp_field_paths.h"

namespace libmphoto {

namespace {

const std::vector<const char *> kHeicFtyps = {
    "ftypmif1", "ftypmsf1", "ftypheic", "ftypheix", "ftyphevc", "ftyphevx"};

}  // namespace

std::unique_ptr<IXmpIOHelper> GetXmpIOHelper(const std::string &image) {
  // Check for FF D8 jpeg header.
  if (image.at(0) == '\377' && image.at(1) == '\330') {
    return absl::make_unique<JpegXmpIOHelper>();
  }

  // Search in first 16 bytes.
  absl::string_view header = absl::string_view(image.data(), 16);

  // Check if any of the heic ftyps are present in header.
  if (std::any_of(kHeicFtyps.begin(), kHeicFtyps.end(),
                  [header](const char *ftyp) {
                    return header.find(ftyp) != std::string::npos;
                  })) {
    return absl::make_unique<HeicXmpIOHelper>();
  }

  return nullptr;
}

MPhotoFormat GetMPhotoFormat(const xmlXPathContext &xpath_context) {
  std::string value;

  if (GetXmlAttributeValue(kMotionPhotoXPath, xpath_context, &value).ok()) {
    return MPhotoFormat::kMotionPhoto;
  } else if (GetXmlAttributeValue(kMicrovideoXPath, xpath_context, &value)
                 .ok()) {
    return MPhotoFormat::kMicrovideo;
  }

  return MPhotoFormat::kNone;
}
}  // namespace libmphoto
