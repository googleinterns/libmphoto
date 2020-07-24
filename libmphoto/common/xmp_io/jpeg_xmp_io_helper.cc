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

#include "libmphoto/common/xmp_io/jpeg_xmp_io_helper.h"

#include <sstream>

#include "xmpmeta/xmp_parser.h"
#include "xmpmeta/xmp_writer.h"
#include "xmpmeta/xmp_data.h"

namespace libmphoto {

std::unique_ptr<xmlDoc, LibXmlDeleter> JpegXmpIOHelper::GetXmp(
    const std::string &image) {
  xmpmeta::XmpData xmp_data;

  if (!xmpmeta::ReadXmpFromMemory(image, true, &xmp_data)) {
    return nullptr;
  }

  // Here we take responsibility of freeing the xmlDoc.
  auto xml_doc =
      std::unique_ptr<xmlDoc, LibXmlDeleter>(xmp_data.StandardSection());
  *xmp_data.MutableStandardSection() = nullptr;
  return xml_doc;
}

absl::Status JpegXmpIOHelper::SetXmp(const xmlDoc &xml_doc,
                                     const std::string &image,
                                     std::string *updated_image) {
  xmpmeta::XmpData xmp_data;

  *xmp_data.MutableStandardSection() = const_cast<xmlDoc *>(&xml_doc);

  std::istringstream istream(image);
  std::ostringstream ostream;

  if (!xmpmeta::AddXmpMetaToJpegStream(&istream, xmp_data, &ostream)) {
    return absl::InternalError("Failed to write xmp metadata to jpeg stream");
  }

  // Take back ownership of the xmlDoc passed in to xmpmeta.
  *xmp_data.MutableStandardSection() = nullptr;

  *updated_image = ostream.str();
  return absl::OkStatus();
}

MimeType JpegXmpIOHelper::GetMimeType() { return MimeType::kImageJpeg; }

}  // namespace libmphoto
