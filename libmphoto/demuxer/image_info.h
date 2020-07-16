// Copyright 2020 Google LLC
// Author: pinheirojamie

#ifndef LIBMPHOTO_DEMUXER_IMAGE_INFO_H_
#define LIBMPHOTO_DEMUXER_IMAGE_INFO_H_

#include <string>
#include <map>

#include "absl/strings/str_format.h"

namespace libmphoto {

// Different possible mime types for containers in motion photo.
enum class MimeType { kUnknownMimeType = 0, kImageJpeg, kImageHeif, kVideoMp4 };

static const std::map<MimeType, std::string> MimeTypeStrings = {
    {MimeType::kUnknownMimeType, "unknown"},
    {MimeType::kImageJpeg, "image/jpeg"},
    {MimeType::kImageHeif, "image/heif"},
    {MimeType::kVideoMp4, "video/mp4"}};

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
  MimeType image_mime_type;

  // MIME type of the media item in the video container.
  MimeType video_mime_type;

  // Byte length of the video container.
  int video_length;

  std::string toString() {
    return absl::StrFormat(
        "Motion Photo: %d\n"
        "Motion Photo Version: %d\n"
        "Motion Photo Presentation Timestamp Us: %lld\n"
        "Image Mime Type: %s\n"
        "Video Mime Type: %s\n"
        "Video Length: %d\n",
        motion_photo, motion_photo_version,
        motion_photo_presentation_timestamp_us,
        MimeTypeStrings.at(image_mime_type),
        MimeTypeStrings.at(video_mime_type), video_length);
  }
};

}  // namespace libmphoto

#endif  // LIBMPHOTO_DEMUXER_IMAGE_INFO_H_
