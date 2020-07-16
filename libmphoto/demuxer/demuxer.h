// Copyright 2020 Google LLC
// Author: pinheirojamie

#ifndef LIBMPHOTO_DEMUXER_DEMUXER_H_
#define LIBMPHOTO_DEMUXER_DEMUXER_H_

#include <memory>
#include <string>
#include <tuple>
#include <utility>

#include "absl/status/status.h"
#include "libxml/parser.h"
#include "libxml/tree.h"
#include "libxml/xpath.h"
#include "libxml/xpathInternals.h"
#include "libmphoto/demuxer/image_info.h"
#include "libmphoto/demuxer/libxml_deleter.h"

namespace libmphoto {

// This class provides functionality for information and encoded media stream
// extraction from a motion photo. Init must first be called before any other
// class functions can be called.
class Demuxer {
 public:
  Demuxer();

  // Initializes the demuxer with a string of bytes representing a motion photo.
  absl::Status Init(const absl::string_view motion_photo);

  // Sets image_info to the ImageInfo for the motion photo.
  absl::Status GetInfo(ImageInfo *image_info);

  // Sets still to the bytes of the still image portion of the motion photo.
  absl::Status GetStill(std::string *still);

  // Sets video to the bytes of the video portion of the motion photo.
  absl::Status GetVideo(std::string *video);

 private:
  std::string motion_photo_;
  std::unique_ptr<ImageInfo> image_info_;
};

}  // namespace libmphoto

#endif  // LIBMPHOTO_DEMUXER_DEMUXER_H_
