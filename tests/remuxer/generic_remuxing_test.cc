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

TEST(GenericRemuxing, CanFailIfStillNotSet) {
  std::string video_bytes = GetBytesFromFile("sample_data/mp4/video.mp4");

  Remuxer remuxer;
  EXPECT_TRUE(remuxer.SetVideo(video_bytes).ok());

  std::string motion_photo;
  EXPECT_EQ(remuxer.Finalize(&motion_photo).code(),
            absl::StatusCode::kFailedPrecondition);
}

TEST(GenericRemuxing, CanFailIfVideoNotSet) {
  std::string still_bytes = GetBytesFromFile("sample_data/jpeg/no_xmp.jpeg");

  Remuxer remuxer;
  EXPECT_TRUE(remuxer.SetStill(still_bytes).ok());

  std::string motion_photo;
  EXPECT_EQ(remuxer.Finalize(&motion_photo).code(),
            absl::StatusCode::kFailedPrecondition);
}

TEST(GenericRemuxing, CanFailIfIncorrectStillType) {
  std::string video_bytes = GetBytesFromFile("sample_data/mp4/video.mp4");

  Remuxer remuxer;
  EXPECT_EQ(remuxer.SetStill(video_bytes).code(),
            absl::StatusCode::kInvalidArgument);
}

TEST(GenericRemuxing, CanFailIfIncorrectVideoType) {
  std::string still_bytes = GetBytesFromFile("sample_data/jpeg/no_xmp.jpeg");

  Remuxer remuxer;
  EXPECT_EQ(remuxer.SetVideo(still_bytes).code(),
            absl::StatusCode::kInvalidArgument);
}

}  // namespace libmphoto
