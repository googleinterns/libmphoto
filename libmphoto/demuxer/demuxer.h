// Copyright 2020 Google LLC
// Author: pinheirojamie

#ifndef LIBMPHOTO_DEMUXER_DEMUXER_H_
#define LIBMPHOTO_DEMUXER_DEMUXER_H_

#include <string>
#include <memory>

#include "absl/status/status.h"
#include "includes/demuxer/image_info.h"

namespace libmphoto {

// This class provides functionality for information and encoded media stream
// extraction from a motion photo. Init must first be called before any other
// class functions can be called.
class Demuxer {
 public:
  Demuxer();

  // Initializes the demuxer with a string of bytes representing a motion photo.
  absl::Status Init(const std::string &motion_photo);

  // Sets image_info to the ImageInfo for the motion photo.
  absl::Status GetInfo(ImageInfo *image_info);

  // Sets still to the bytes of the still image portion of the motion photo.
  absl::Status GetStill(std::string *still);

  // Sets video to the bytes of the video portion of the motion photo.
  absl::Status GetVideo(std::string *video);

 private:
  const std::string &motion_photo_;
  std::unique_ptr<ImageInfo> image_info_;
};

}  // namespace libmphoto

#endif  // LIBMPHOTO_DEMUXER_DEMUXER_H_
