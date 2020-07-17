// Copyright 2020 Google LLC
// Author: pinheirojamie

#include <fstream>
#include <iostream>
#include <vector>

#include "gtest/gtest.h"
#include "libmphoto/demuxer/demuxer.h"
#include "tests/common/io_helper.h"

namespace libmphoto {

TEST(StillDemuxing, CanDemuxAValidMotionPhoto) {
  std::string motion_photo_bytes =
      GetBytesFromFile("sample_data/motionphoto.jpg");
  std::string correct_still_bytes =
      GetBytesFromFile("sample_data/motionphoto_still.jpg");

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
