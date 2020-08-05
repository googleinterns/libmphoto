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

#ifndef LIBMPHOTO_COMMON_XMP_IO_LIBHEIF_DELETER_H_
#define LIBMPHOTO_COMMON_XMP_IO_LIBHEIF_DELETER_H_

#include "libheif/heif.h"

namespace libmphoto {

struct LibHeifDeleter {
  void operator()(heif_context *heif_context) {
    heif_context_free(heif_context);
  }
  void operator()(heif_image_handle *heif_image_handle) {
    heif_image_handle_release(heif_image_handle);
  }
};

}  // namespace libmphoto

#endif  // LIBMPHOTO_COMMON_XMP_IO_LIBHEIF_DELETER_H_
