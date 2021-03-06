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

#ifndef LIBMPHOTO_DEMUXER_IMAGE_INFO_H_
#define LIBMPHOTO_DEMUXER_IMAGE_INFO_H_

#include <string>
#include <map>

#include "absl/strings/str_format.h"
#include "libmphoto/common/mime_type.h"

namespace libmphoto {

// This struct holds the metadata information from a motion photo.
struct ImageInfo {
  // Describes how the file is treated. 1 if motion photo, otherwise 0.
  int motion_photo;

  // Indicates the file format version of the Motion Photo.
  // Todo(nchusid) Establish convention for motion photo versioning
  int motion_photo_version;

  // Represents the presentation timestamp, in microseconds, of the video frame
  // that corresponds to the image still. This value can be -1 to denote unset
  // or unspecified.
  int64_t motion_photo_presentation_timestamp_us;

  // MIME type of the media item in the still image container.
  MimeType still_mime_type;

  // MIME type of the media item in the video container.
  MimeType video_mime_type;

  // Byte length of the video container.
  int video_length;

  // Byte length of padding after the still container.
  int still_padding;

  std::string toString() {
    return absl::StrFormat(
        "Motion Photo: %d\n"
        "Motion Photo Version: %d\n"
        "Motion Photo Presentation Timestamp Us: %lld\n"
        "Still Mime Type: %s\n"
        "Video Mime Type: %s\n"
        "Video Length: %d\n"
        "Still Padding: %d",
        motion_photo, motion_photo_version,
        motion_photo_presentation_timestamp_us,
        kMimeTypeToString.at(still_mime_type),
        kMimeTypeToString.at(video_mime_type), video_length, still_padding);
  }
};

}  // namespace libmphoto

#endif  // LIBMPHOTO_DEMUXER_IMAGE_INFO_H_
