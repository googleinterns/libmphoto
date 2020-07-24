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

#include "libmphoto/common/xmp_io/heic_xmp_io_helper.h"

#include "libxml/parser.h"

namespace libmphoto {

namespace {

constexpr char kStartXmpMetadata[] = "<x:xmpmeta";
constexpr char kEndXmpMetadata[] = "</x:xmpmeta>";

}  // namespace

// Search for the start and end tags of xmp metadata anywhere in the image. If
// found, attempts to parse it with libxml.
std::unique_ptr<xmlDoc, LibXmlDeleter> HeicXmpIOHelper::GetXmp(
    const std::string &image) {
  std::size_t start_pos = image.find(kStartXmpMetadata);
  std::size_t end_pos = image.find(kEndXmpMetadata) + strlen(kEndXmpMetadata);

  if (start_pos == std::string::npos || end_pos == std::string::npos) {
    return nullptr;
  }

  std::string xmp_data = image.substr(start_pos, end_pos + 1);

  return std::unique_ptr<xmlDoc, LibXmlDeleter>(
      xmlReadMemory(xmp_data.c_str(), end_pos - start_pos, ".xml", nullptr, 0));
}

absl::Status HeicXmpIOHelper::SetXmp(const xmlDoc &xml_doc,
                                     const std::string &image,
                                     std::string *updated_image) {
  return absl::UnimplementedError("");
}

MimeType HeicXmpIOHelper::GetMimeType() { return MimeType::kImageHeic; }

}  // namespace libmphoto
