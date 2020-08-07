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

#ifndef LIBMPHOTO_COMMON_STREAM_PARSER_H_
#define LIBMPHOTO_COMMON_STREAM_PARSER_H_

#include <string>

#include "absl/strings/string_view.h"
#include "libmphoto/common/mime_type.h"

namespace libmphoto {

// Returns the stream's mime type based on its header.
MimeType GetStreamMimeType(const absl::string_view stream);

}  // namespace libmphoto

#endif  // LIBMPHOTO_COMMON_STREAM_PARSER_H_
