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

#include "gtest/gtest.h"
#include "libmphoto/demuxer/demuxer.h"
#include "tests/common/io_helper.h"

namespace libmphoto {

TEST(StillDemuxing, CanDemuxAValidHeicMotionPhoto) {
  std::string motion_photo_bytes =
      GetBytesFromFile("sample_data/heic_motion_photo/motion_photo.heic");
  std::string correct_still_bytes =
      GetBytesFromFile("sample_data/heic_motion_photo/still.heic");

  Demuxer demuxer;
  EXPECT_TRUE(demuxer.Init(motion_photo_bytes).ok());

  std::string demuxed_still_bytes;
  EXPECT_TRUE(demuxer.GetStill(&demuxed_still_bytes).ok());
  EXPECT_EQ(demuxed_still_bytes, correct_still_bytes) << "Bytes differ";
}

TEST(StillDemuxing, CanDemuxAValidJpegMotionPhoto) {
  std::string motion_photo_bytes =
      GetBytesFromFile("sample_data/jpeg_motion_photo/motion_photo.jpeg");
  std::string correct_still_bytes =
      GetBytesFromFile("sample_data/jpeg_motion_photo/still.jpeg");

  Demuxer demuxer;
  EXPECT_TRUE(demuxer.Init(motion_photo_bytes).ok());

  std::string demuxed_still_bytes;
  EXPECT_TRUE(demuxer.GetStill(&demuxed_still_bytes).ok());
  EXPECT_EQ(demuxed_still_bytes, correct_still_bytes) << "Bytes differ";
}

TEST(StillDemuxing, CanDemuxAValidMicrovideo) {
  std::string microvideo_bytes =
      GetBytesFromFile("sample_data/microvideo/microvideo.jpeg");
  std::string correct_still_bytes =
      GetBytesFromFile("sample_data/microvideo/still.jpeg");

  Demuxer demuxer;
  EXPECT_TRUE(demuxer.Init(microvideo_bytes).ok());

  std::string demuxed_still_bytes;
  EXPECT_TRUE(demuxer.GetStill(&demuxed_still_bytes).ok());
  EXPECT_EQ(demuxed_still_bytes, correct_still_bytes) << "Bytes differ";
}

TEST(StillDemuxing, CanFailWhenDemuxerNotInitialzed) {
  Demuxer demuxer;
  std::string demuxed_still_bytes;

  EXPECT_EQ(demuxer.GetStill(&demuxed_still_bytes).code(),
            absl::StatusCode::kFailedPrecondition);
}

}  // namespace libmphoto
