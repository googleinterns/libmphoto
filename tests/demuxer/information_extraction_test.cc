// Copyright 2020 Google LLC
// Author: pinheirojamie

#include <fstream>
#include <iostream>
#include <vector>

#include "gtest/gtest.h"
#include "absl/strings/str_format.h"
#include "libmphoto/demuxer/demuxer.h"
#include "libmphoto/demuxer/image_info.h"

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

// Returns the bytes of a file read.
std::string GetPhotoBytesFromFile(const std::string &file_name) {
  std::ifstream file(file_name, std::ifstream::in | std::ifstream::binary);
  file.seekg(0, std::ios::end);
  std::streampos length = file.tellg();
  file.seekg(0, std::ios::beg);
  std::vector<uint8_t> data(length);
  file.read((char *)data.data(), length);
  return std::string(data.begin(), data.end());
}

// Returns the bytes for a motion photo with the xmp metadata embedded inside.
std::string GetPhotoBytesFromXmp(const std::string &xmp_metadata) {
  char pre_xmp_bytes[1000], post_xmp_bytes[5000];
  return pre_xmp_bytes + xmp_metadata + post_xmp_bytes;
}

}  // namespace

TEST(InformationExtraction, CanParseValidMotionPhoto) {
  std::string photo_bytes = GetPhotoBytesFromFile("images/motionphoto.jpg");

  libmphoto::Demuxer demuxer;
  libmphoto::ImageInfo image_info;

  EXPECT_TRUE(demuxer.Init(photo_bytes).ok());
  EXPECT_TRUE(demuxer.GetInfo(&image_info).ok());

  EXPECT_EQ(image_info.motion_photo, 1);
  EXPECT_EQ(image_info.motion_photo_version, 1);
  EXPECT_EQ(image_info.motion_photo_presentation_timestamp_us, 500000);
  EXPECT_EQ(image_info.image_mime_type, libmphoto::MimeType::kImageJpeg);
  EXPECT_EQ(image_info.video_mime_type, libmphoto::MimeType::kVideoMp4);
  EXPECT_EQ(image_info.video_length, 122562);
}

TEST(InformationExtraction, CanParseValidMicrovideo) {
  std::string photo_bytes = GetPhotoBytesFromFile("images/microvideo.jpg");

  libmphoto::Demuxer demuxer;
  libmphoto::ImageInfo image_info;

  EXPECT_TRUE(demuxer.Init(photo_bytes).ok());
  EXPECT_TRUE(demuxer.GetInfo(&image_info).ok());

  EXPECT_EQ(image_info.motion_photo, 1);
  EXPECT_EQ(image_info.motion_photo_version, 1);
  EXPECT_EQ(image_info.motion_photo_presentation_timestamp_us, -1);
  EXPECT_EQ(image_info.image_mime_type, libmphoto::MimeType::kImageJpeg);
  EXPECT_EQ(image_info.video_mime_type, libmphoto::MimeType::kVideoMp4);
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

  libmphoto::Demuxer demuxer;
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

  libmphoto::Demuxer demuxer;
  EXPECT_EQ(demuxer.Init(photo_bytes).code(), absl::StatusCode::kNotFound);
}

TEST(InformationExtraction, CanFailWhenInvalidDataTypeXmpFields) {
  // Motion photo version should be type integer.
  std::string wrong_type_xmp_metadata =
      GetXmp("1", "latest", "500000", "image/jpeg", "video/mp4", "122562");

  std::string photo_bytes = GetPhotoBytesFromXmp(wrong_type_xmp_metadata);

  libmphoto::Demuxer demuxer;
  EXPECT_EQ(demuxer.Init(photo_bytes).code(),
            absl::StatusCode::kInvalidArgument);
}

TEST(InformationExtraction, CanFailWhenInvalidMimeType) {
  // Mime type of image/png is not valid for a motion photo.
  std::string invalid_mime_type_xmp_metadata =
      GetXmp("1", "latest", "500000", "image/png", "video/mp4", "122562");
  std::string photo_bytes =
      GetPhotoBytesFromXmp(invalid_mime_type_xmp_metadata);

  libmphoto::Demuxer demuxer;
  EXPECT_EQ(demuxer.Init(photo_bytes).code(),
            absl::StatusCode::kInvalidArgument);
}

TEST(InformationExtraction, CanFailWhenNotMotionPhoto) {
  // Motion photo = 0 signifies not to treat the file as a motion photo.
  std::string invalid_mime_type_xmp_metadata =
      GetXmp("0", "latest", "500000", "image/jpeg", "video/mp4", "122562");

  std::string photo_bytes =
      GetPhotoBytesFromXmp(invalid_mime_type_xmp_metadata);

  libmphoto::Demuxer demuxer;
  EXPECT_EQ(demuxer.Init(photo_bytes).code(),
            absl::StatusCode::kInvalidArgument);
}

TEST(InformationExtraction, CanFailWhenInvalidLength) {
  // Video length not possible as it's larger than the file.
  std::string invalid_mime_type_xmp_metadata =
      GetXmp("0", "latest", "500000", "image/jpeg", "video/mp4", "2147483647");

  std::string photo_bytes =
      GetPhotoBytesFromXmp(invalid_mime_type_xmp_metadata);

  libmphoto::Demuxer demuxer;
  EXPECT_EQ(demuxer.Init(photo_bytes).code(),
            absl::StatusCode::kInvalidArgument);
}
