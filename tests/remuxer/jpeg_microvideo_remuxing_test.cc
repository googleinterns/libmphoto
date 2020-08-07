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
#include "libmphoto/remuxer/remuxer.h"
#include "tests/common/io_helper.h"

namespace libmphoto {

TEST(JpegMicrovideoRemuxing, CanRemuxWithOnlyMicrovideoXmp) {
  std::string still_bytes =
      GetBytesFromFile("sample_data/microvideo/still.jpeg");
  std::string video_bytes = GetBytesFromFile("sample_data/mp4/video.mp4");

  Remuxer remuxer;
  EXPECT_TRUE(remuxer.SetStill(still_bytes, 10).ok());
  EXPECT_TRUE(remuxer.SetVideo(video_bytes).ok());

  std::string motion_photo;
  EXPECT_TRUE(remuxer.Finalize(&motion_photo).ok());

  std::string expected_remuxed_bytes =
      GetBytesFromFile("sample_data/remuxed/jpeg/microvideo_xmp.jpeg");

  EXPECT_EQ(motion_photo, expected_remuxed_bytes) << "Bytes differ";

  Demuxer demuxer;
  EXPECT_TRUE(demuxer.Init(motion_photo).ok());

  ImageInfo image_info;
  EXPECT_TRUE(demuxer.GetInfo(&image_info).ok());

  EXPECT_EQ(image_info.motion_photo, 1);
  EXPECT_EQ(image_info.motion_photo_version, 1);
  EXPECT_EQ(image_info.motion_photo_presentation_timestamp_us, 10);
  EXPECT_EQ(image_info.still_mime_type, MimeType::kImageJpeg);
  EXPECT_EQ(image_info.video_mime_type, MimeType::kVideoMp4);
  EXPECT_EQ(image_info.video_length, video_bytes.length());
}

}  // namespace libmphoto
