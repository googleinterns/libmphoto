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

#include <fstream>
#include <iostream>
#include <vector>

#include "gtest/gtest.h"
#include "libmphoto/demuxer/demuxer.h"
#include "tests/common/io_helper.h"

namespace libmphoto {

TEST(StillDemuxing, CanDemuxAValidHeifMotionPhoto) {
  std::string motion_photo_bytes =
      GetBytesFromFile("sample_data/motionphoto_heif.heic");
  std::string correct_still_bytes =
      GetBytesFromFile("sample_data/motionphoto_heif_still.heic");

  Demuxer demuxer;
  EXPECT_TRUE(demuxer.Init(motion_photo_bytes).ok());

  std::string demuxed_still_bytes;
  EXPECT_TRUE(demuxer.GetStill(&demuxed_still_bytes).ok());
  EXPECT_EQ(demuxed_still_bytes, correct_still_bytes);
}

TEST(StillDemuxing, CanDemuxAValidJpegMotionPhoto) {
  std::string motion_photo_bytes =
      GetBytesFromFile("sample_data/motionphoto_jpeg.jpg");
  std::string correct_still_bytes =
      GetBytesFromFile("sample_data/motionphoto_jpeg_still.jpg");

  Demuxer demuxer;
  EXPECT_TRUE(demuxer.Init(motion_photo_bytes).ok());

  std::string demuxed_still_bytes;
  EXPECT_TRUE(demuxer.GetStill(&demuxed_still_bytes).ok());
  EXPECT_EQ(demuxed_still_bytes, correct_still_bytes);
}

TEST(StillDemuxing, CanDemuxAValidMicrovideo) {
  std::string microvideo_bytes = GetBytesFromFile("sample_data/microvideo.jpg");
  std::string correct_still_bytes =
      GetBytesFromFile("sample_data/microvideo_still.jpg");

  Demuxer demuxer;
  EXPECT_TRUE(demuxer.Init(microvideo_bytes).ok());

  std::string demuxed_still_bytes;
  EXPECT_TRUE(demuxer.GetStill(&demuxed_still_bytes).ok());
  EXPECT_EQ(demuxed_still_bytes, correct_still_bytes);
}

TEST(StillDemuxing, CanFailWhenDemuxerNotInitialzed) {
  Demuxer demuxer;
  std::string demuxed_still_bytes;

  EXPECT_EQ(demuxer.GetStill(&demuxed_still_bytes).code(),
            absl::StatusCode::kFailedPrecondition);
}

}  // namespace libmphoto
