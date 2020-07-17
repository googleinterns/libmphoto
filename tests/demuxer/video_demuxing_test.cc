// Copyright 2020 Google LLC
// Author: pinheirojamie

#include <fstream>
#include <iostream>
#include <vector>

#include "gtest/gtest.h"
#include "libmphoto/demuxer/demuxer.h"
#include "tests/common/io_helper.h"

namespace libmphoto {

TEST(VideoDemuxing, CanDemuxAValidMotionPhoto) {
  std::string motion_photo_bytes =
      GetBytesFromFile("sample_data/motionphoto.jpg");
  std::string correct_video_bytes =
      GetBytesFromFile("sample_data/motionphoto_video.mp4");

  Demuxer demuxer;
  EXPECT_TRUE(demuxer.Init(motion_photo_bytes).ok());

  std::string demuxed_video_bytes;
  EXPECT_TRUE(demuxer.GetVideo(&demuxed_video_bytes).ok());
  EXPECT_EQ(demuxed_video_bytes, correct_video_bytes);
}

TEST(VideoDemuxing, CanDemuxAValidMicrovideo) {
  std::string microvideo_bytes = GetBytesFromFile("sample_data/microvideo.jpg");
  std::string correct_video_bytes =
      GetBytesFromFile("sample_data/microvideo_video.mp4");

  Demuxer demuxer;
  EXPECT_TRUE(demuxer.Init(microvideo_bytes).ok());

  std::string demuxed_video_bytes;
  EXPECT_TRUE(demuxer.GetVideo(&demuxed_video_bytes).ok());
  EXPECT_EQ(demuxed_video_bytes, correct_video_bytes);
}

TEST(VideoDemuxing, CanFailWhenDemuxerNotInitialzed) {
  Demuxer demuxer;
  std::string demuxed_video_bytes;

  EXPECT_EQ(demuxer.GetVideo(&demuxed_video_bytes).code(),
            absl::StatusCode::kFailedPrecondition);
}

}  // namespace libmphoto
