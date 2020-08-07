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

#include "libmphoto/common/stream_parser.h"

#include <vector>
#include <algorithm>

#include "absl/strings/string_view.h"

namespace libmphoto {

namespace {

constexpr int header_size = 16;

const std::vector<const char *> kHeicFtyps = {
    "ftypmif1", "ftypmsf1", "ftypheic", "ftypheix", "ftyphevc", "ftyphevx"};

const std::vector<const char *> kMp4Ftyps = {
    "avc1", "iso2", "isom", "mmp4", "mp41", "mp42", "mp71",
    "msnv", "ndas", "ndsc", "ndsh", "ndsm", "ndsp", "ndss",
    "ndxc", "ndxh", "ndxm", "ndxp", "ndxs"};

bool AnyFtypPresentInHeader(absl::string_view header,
                            std::vector<const char *> ftyps) {
  return std::any_of(ftyps.begin(), ftyps.end(), [header](const char *ftyp) {
    return header.find(ftyp) != std::string::npos;
  });
}

}  // namespace

MimeType GetStreamMimeType(const absl::string_view stream) {
  // Check for FF D8 jpeg header.
  if (stream.at(0) == '\xFF' && stream.at(1) == '\xD8') {
    return MimeType::kImageJpeg;
  }

  absl::string_view header = absl::string_view(stream.data(), header_size);

  if (AnyFtypPresentInHeader(header, kHeicFtyps)) {
    return MimeType::kImageHeic;
  }

  if (AnyFtypPresentInHeader(header, kMp4Ftyps)) {
    return MimeType::kVideoMp4;
  }

  return MimeType::kUnknownMimeType;
}

}  // namespace libmphoto
