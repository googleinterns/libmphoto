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
#include "absl/strings/str_format.h"
#include "libmphoto/demuxer/demuxer.h"
#include "libmphoto/demuxer/image_info.h"
#include "tests/common/io_helper.h"

namespace {

// Generates motion photo xmp metadata based on inputs.
std::string GetXmp(const std::string &motion_photo,
                   const std::string &motion_photo_version,
                   const std::string &timestamp,
                   const std::string &image_meme_type,
                   const std::string &video_meme_type,
                   const std::string &video_length) {
  return absl::StrFormat(
      "<x:xmpmeta\n"
      "  xmlns:x=\"adobe:ns:meta/\"\n"
      "  x:xmptk=\"Adobe XMP Core 5.1.0-jc003\">\n"
      "  <rdf:RDF\n"
      "    xmlns:rdf=\"http://www.w3.org/1999/02/22-rdf-syntax-ns#\">\n"
      "    <rdf:Description\n"
      "      rdf:about=\"\"\n"
      "      xmlns:Camera=\"http://ns.google.com/photos/1.0/camera/\"\n"
      "      xmlns:Container=\"http://ns.google.com/photos/1.0/container/\"\n"
      "      xmlns:Item=\"http://ns.google.com/photos/1.0/container/item/\"\n"
      "      Camera:MotionPhoto=\"%s\"\n"
      "      Camera:MotionPhotoVersion=\"%s\"\n"
      "      Camera:MotionPhotoPresentationTimestampUs=\"%s\"\n"
      "      Container:Version=\"1\">\n"
      "      <Container:Directory>\n"
      "        <rdf:Seq>\n"
      "          <rdf:li>\n"
      "            <Container:Item\n"
      "              Item:Semantic=\"Primary\"\n"
      "              Item:Mime=\"%s\"/>\n"
      "          </rdf:li>\n"
      "          <rdf:li>\n"
      "            <Container:Item\n"
      "              Item:Semantic=\"MotionPhoto\"\n"
      "              Item:Mime=\"%s\"\n"
      "              Item:Length=\"%s\"/>\n"
      "          </rdf:li>\n"
      "        </rdf:Seq>\n"
      "      </Container:Directory>\n"
      "    </rdf:Description>\n"
      "  </rdf:RDF>\n"
      "</x:xmpmeta>",
      motion_photo, motion_photo_version, timestamp, image_meme_type,
      video_meme_type, video_length);
}

// Returns the bytes for a motion photo with the xmp metadata embedded inside.
std::string GetPhotoBytesFromXmp(const std::string &xmp_metadata) {
  char pre_xmp_bytes[1000], post_xmp_bytes[5000];
  return pre_xmp_bytes + xmp_metadata + post_xmp_bytes;
}

}  // namespace

namespace libmphoto {

TEST(InformationExtraction, CanParseValidHeicMotionPhoto) {
  std::string photo_bytes =
      GetBytesFromFile("sample_data/motionphoto_heic.heic");

  Demuxer demuxer;
  ImageInfo image_info;

  EXPECT_TRUE(demuxer.Init(photo_bytes).ok());
  EXPECT_TRUE(demuxer.GetInfo(&image_info).ok());

  EXPECT_EQ(image_info.motion_photo, 1);
  EXPECT_EQ(image_info.motion_photo_version, 1);
  EXPECT_EQ(image_info.motion_photo_presentation_timestamp_us, 0);
  EXPECT_EQ(image_info.image_mime_type, MimeType::kImageHeic);
  EXPECT_EQ(image_info.video_mime_type, MimeType::kVideoMp4);
  EXPECT_EQ(image_info.video_length, 1544201);
}

TEST(InformationExtraction, CanParseValidJpegMotionPhoto) {
  std::string photo_bytes =
      GetBytesFromFile("sample_data/motionphoto_jpeg.jpg");

  Demuxer demuxer;
  ImageInfo image_info;

  EXPECT_TRUE(demuxer.Init(photo_bytes).ok());
  EXPECT_TRUE(demuxer.GetInfo(&image_info).ok());

  EXPECT_EQ(image_info.motion_photo, 1);
  EXPECT_EQ(image_info.motion_photo_version, 1);
  EXPECT_EQ(image_info.motion_photo_presentation_timestamp_us, 500000);
  EXPECT_EQ(image_info.image_mime_type, MimeType::kImageJpeg);
  EXPECT_EQ(image_info.video_mime_type, MimeType::kVideoMp4);
  EXPECT_EQ(image_info.video_length, 122562);
}

TEST(InformationExtraction, CanParseValidMicrovideo) {
  std::string photo_bytes = GetBytesFromFile("sample_data/microvideo.jpg");

  Demuxer demuxer;
  ImageInfo image_info;

  EXPECT_TRUE(demuxer.Init(photo_bytes).ok());
  EXPECT_TRUE(demuxer.GetInfo(&image_info).ok());

  EXPECT_EQ(image_info.motion_photo, 1);
  EXPECT_EQ(image_info.motion_photo_version, 1);
  EXPECT_EQ(image_info.motion_photo_presentation_timestamp_us, -1);
  EXPECT_EQ(image_info.image_mime_type, MimeType::kImageJpeg);
  EXPECT_EQ(image_info.video_mime_type, MimeType::kVideoMp4);
  EXPECT_EQ(image_info.video_length, 4182318);
}

TEST(InformationExtraction, CanFailOnInvalidXmp) {
  std::string corrupt_xmp_metadata =
      "<x:xmpmeta\n"
      "  xmlns:x=\"adobe:ns:meta/\"\n"
      "  x:xmptk=\"Adobe XMP Core 5.1.0-jc003\">\n"
      // Removed lines here
      "      Camera:MotionPhoto=\"1\"\n"
      "      Camera:MotionPhotoVersion=\"1\"\n"
      "      Camera:MotionPhotoPresentationTimestampUs=\"500000\"\n"
      "      Container:Version=\"1\">\n"
      "      <Container:Directory>\n"
      "        <rdf:Seq>\n"
      "          <rdf:li>\n"
      "            <Container:Item\n"
      "              Item:Semantic=\"Primary\"\n"
      "              Item:Mime=\"image/jpeg\"/>\n"
      "          </rdf:li>\n"
      "          <rdf:li>\n"
      "            <Container:Item\n"
      "              Item:Semantic=\"MotionPhoto\"\n"
      "              Item:Mime=\"video/mp4\"\n"
      "              Item:Length=\"122562\"/>\n"
      "          </rdf:li>\n"
      "        </rdf:Seq>\n"
      "      </Container:Directory>\n"
      "    </rdf:Description>\n"
      "  </rdf:RDF>\n"
      "</x:xmpmeta>";

  std::string photo_bytes = GetPhotoBytesFromXmp(corrupt_xmp_metadata);

  Demuxer demuxer;
  EXPECT_EQ(demuxer.Init(photo_bytes).code(),
            absl::StatusCode::kInvalidArgument);
}

TEST(InformationExtraction, CanFailWhenMissingXmpFields) {
  std::string missing_field_xmp_metadata =
      "<x:xmpmeta\n"
      "  xmlns:x=\"adobe:ns:meta/\"\n"
      "  x:xmptk=\"Adobe XMP Core 5.1.0-jc003\">\n"
      "  <rdf:RDF\n"
      "    xmlns:rdf=\"http://www.w3.org/1999/02/22-rdf-syntax-ns#\">\n"
      "    <rdf:Description\n"
      "      rdf:about=\"\"\n"
      "      xmlns:Camera=\"http://ns.google.com/photos/1.0/camera/\"\n"
      "      xmlns:Container=\"http://ns.google.com/photos/1.0/container/\"\n"
      "      xmlns:Item=\"http://ns.google.com/photos/1.0/container/item/\"\n"
      "      Camera:MotionPhoto=\"1\"\n"
      /*"      Camera:MotionPhotoVersion=\"1\"\n"*/
      "      Camera:MotionPhotoPresentationTimestampUs=\"500000\"\n"
      "      Container:Version=\"1\">\n"
      "      <Container:Directory>\n"
      "        <rdf:Seq>\n"
      "          <rdf:li>\n"
      "            <Container:Item\n"
      "              Item:Semantic=\"Primary\"\n"
      "              Item:Mime=\"image/jpeg\"/>\n"
      "          </rdf:li>\n"
      "          <rdf:li>\n"
      "            <Container:Item\n"
      "              Item:Semantic=\"MotionPhoto\"\n"
      "              Item:Mime=\"video/mp4\"\n"
      "              Item:Length=\"122562\"/>\n"
      "          </rdf:li>\n"
      "        </rdf:Seq>\n"
      "      </Container:Directory>\n"
      "    </rdf:Description>\n"
      "  </rdf:RDF>\n"
      "</x:xmpmeta>";

  std::string photo_bytes = GetPhotoBytesFromXmp(missing_field_xmp_metadata);

  Demuxer demuxer;
  EXPECT_EQ(demuxer.Init(photo_bytes).code(), absl::StatusCode::kNotFound);
}

TEST(InformationExtraction, CanFailWhenInvalidDataTypeXmpFields) {
  // Motion photo version should be type integer.
  std::string wrong_type_xmp_metadata =
      GetXmp("1", "latest", "500000", "image/jpeg", "video/mp4", "122562");

  std::string photo_bytes = GetPhotoBytesFromXmp(wrong_type_xmp_metadata);

  Demuxer demuxer;
  EXPECT_EQ(demuxer.Init(photo_bytes).code(),
            absl::StatusCode::kInvalidArgument);
}

TEST(InformationExtraction, CanFailWhenInvalidMimeType) {
  // Mime type of image/png is not valid for a motion photo.
  std::string invalid_mime_type_xmp_metadata =
      GetXmp("1", "latest", "500000", "image/png", "video/mp4", "122562");
  std::string photo_bytes =
      GetPhotoBytesFromXmp(invalid_mime_type_xmp_metadata);

  Demuxer demuxer;
  EXPECT_EQ(demuxer.Init(photo_bytes).code(),
            absl::StatusCode::kInvalidArgument);
}

TEST(InformationExtraction, CanFailWhenNotMotionPhoto) {
  // Motion photo = 0 signifies not to treat the file as a motion photo.
  std::string invalid_mime_type_xmp_metadata =
      GetXmp("0", "latest", "500000", "image/jpeg", "video/mp4", "122562");

  std::string photo_bytes =
      GetPhotoBytesFromXmp(invalid_mime_type_xmp_metadata);

  Demuxer demuxer;
  EXPECT_EQ(demuxer.Init(photo_bytes).code(),
            absl::StatusCode::kInvalidArgument);
}

TEST(InformationExtraction, CanFailWhenInvalidLength) {
  // Video length not possible as it's larger than the file.
  std::string invalid_mime_type_xmp_metadata =
      GetXmp("0", "latest", "500000", "image/jpeg", "video/mp4", "2147483647");

  std::string photo_bytes =
      GetPhotoBytesFromXmp(invalid_mime_type_xmp_metadata);

  Demuxer demuxer;
  EXPECT_EQ(demuxer.Init(photo_bytes).code(),
            absl::StatusCode::kInvalidArgument);
}

}  // namespace libmphoto
