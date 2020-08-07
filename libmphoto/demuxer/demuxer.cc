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

#include "libmphoto/demuxer/demuxer.h"

#include <map>
#include <tuple>
#include <utility>

#include "absl/strings/numbers.h"
#include "absl/strings/ascii.h"
#include "libmphoto/common/macros.h"
#include "libmphoto/common/xmp_field_paths.h"
#include "libmphoto/common/xml/libxml_deleter.h"
#include "libmphoto/common/xml/xml_utils.h"
#include "libmphoto/common/xmp_io/xmp_io_helper.h"
#include "libmphoto/common/stream_parser.h"

namespace libmphoto {

namespace {

const absl::Status kDemuxerNotInitializedError =
    absl::FailedPreconditionError("Demuxer has not been initialized");
const absl::Status kInvalidMotionPhotoError =
    absl::InvalidArgumentError("Invalid motion photo bytes");
const absl::Status kOutPtrIsNullError =
    absl::InvalidArgumentError("Out Pointer is null");
const absl::Status kIncorrectTypeError =
    absl::InvalidArgumentError("Incorrect xml attribute type");

const std::map<std::string, MimeType> kStringToMimeType = {
    {"image/jpeg", MimeType::kImageJpeg},
    {"image/jpg", MimeType::kImageJpeg},
    {"image/heic", MimeType::kImageHeic},
    {"video/mp4", MimeType::kVideoMp4}};

MimeType GetMimeType(const std::string &input) {
  std::string lower_case_input = absl::AsciiStrToLower(input);

  if (kStringToMimeType.find(lower_case_input) != kStringToMimeType.end()) {
    return kStringToMimeType.at(lower_case_input);
  }

  return MimeType::kUnknownMimeType;
}

absl::Status GetImageInfoFromMotionPhoto(const xmlXPathContext &xpath_context,
                                         ImageInfo *image_info) {
  std::string value;

  RETURN_IF_ERROR(
      GetXmlAttributeValue(kMotionPhotoXPath, xpath_context, &value));
  if (!absl::SimpleAtoi(value, &image_info->motion_photo)) {
    return kIncorrectTypeError;
  }

  RETURN_IF_ERROR(
      GetXmlAttributeValue(kMotionPhotoVersionXPath, xpath_context, &value));
  if (!absl::SimpleAtoi(value, &image_info->motion_photo_version)) {
    return kIncorrectTypeError;
  }

  RETURN_IF_ERROR(GetXmlAttributeValue(kMotionPhotoPresentationTimestampUsXPath,
                                       xpath_context, &value));
  if (!absl::SimpleAtoi(value,
                        &image_info->motion_photo_presentation_timestamp_us)) {
    return kIncorrectTypeError;
  }

  RETURN_IF_ERROR(
      GetXmlAttributeValue(kImageMimeTypeXPath, xpath_context, &value));
  image_info->still_mime_type = GetMimeType(value);

  RETURN_IF_ERROR(
      GetXmlAttributeValue(kVideoMimeTypeXPath, xpath_context, &value));
  image_info->video_mime_type = GetMimeType(value);

  RETURN_IF_ERROR(
      GetXmlAttributeValue(kVideoLengthXPath, xpath_context, &value));
  if (!absl::SimpleAtoi(value, &image_info->video_length)) {
    return kIncorrectTypeError;
  }

  // Still padding is optional, attempt to parse to see if it is present,
  // otherwise padding is 0.
  if (GetXmlAttributeValue(kStillPaddingXPath, xpath_context, &value).ok()) {
    if (!absl::SimpleAtoi(value, &image_info->still_padding)) {
      return kIncorrectTypeError;
    }
  } else {
    image_info->still_padding = 0;
  }

  return absl::OkStatus();
}

absl::Status GetImageInfoFromMicrovideo(const xmlXPathContext &xpath_context,
                                        ImageInfo *image_info) {
  std::string value;

  RETURN_IF_ERROR(
      GetXmlAttributeValue(kMicrovideoXPath, xpath_context, &value));
  if (!absl::SimpleAtoi(value, &image_info->motion_photo)) {
    return kIncorrectTypeError;
  }

  RETURN_IF_ERROR(
      GetXmlAttributeValue(kMicrovideoVersionXPath, xpath_context, &value));
  if (!absl::SimpleAtoi(value, &image_info->motion_photo_version)) {
    return kIncorrectTypeError;
  }

  RETURN_IF_ERROR(GetXmlAttributeValue(kMicrovideoPresentationTimestampUsXPath,
                                       xpath_context, &value));
  if (!absl::SimpleAtoi(value,
                        &image_info->motion_photo_presentation_timestamp_us)) {
    return kIncorrectTypeError;
  }

  // Image/Video Mime Type are as specified in Microvideo spec.
  image_info->still_mime_type = MimeType::kImageJpeg;
  image_info->video_mime_type = MimeType::kVideoMp4;

  RETURN_IF_ERROR(
      GetXmlAttributeValue(kMicrovideoOffsetXPath, xpath_context, &value));
  if (!absl::SimpleAtoi(value, &image_info->video_length)) {
    return kIncorrectTypeError;
  }

  // Microvideos do not have padding after the still.
  image_info->still_padding = 0;

  return absl::OkStatus();
}

absl::Status GetImageInfo(const xmlDoc &xml_doc, ImageInfo *image_info) {
  auto xpath_context =
      GetXPathContext(kNamespaces, const_cast<xmlDoc *>(&xml_doc));
  if (!xpath_context) {
    return kFailedXPathCreationError;
  }

  MPhotoFormat format = GetMPhotoFormat(*xpath_context);

  if (format == MPhotoFormat::kMotionPhoto) {
    return GetImageInfoFromMotionPhoto(*xpath_context, image_info);
  } else if (format == MPhotoFormat::kMicrovideo) {
    return GetImageInfoFromMicrovideo(*xpath_context, image_info);
  }

  return kInvalidMotionPhotoError;
}

absl::Status ValidateImageInfo(const ImageInfo &image_info,
                               const absl::string_view still,
                               const absl::string_view video,
                               const std::string &motion_photo) {
  if (image_info.motion_photo != 1) {
    return absl::InvalidArgumentError(absl::StrFormat(
        "Motion Photo field set to %d, must be 1", image_info.motion_photo));
  }

  if (image_info.video_length <= 0 ||
      image_info.video_length * 1ul > motion_photo.length()) {
    return absl::InvalidArgumentError("Video length is invalid: " +
                                      image_info.video_length);
  }

  if (image_info.still_mime_type == MimeType::kUnknownMimeType) {
    return absl::InvalidArgumentError("Invalid image mime type");
  }

  if (image_info.still_mime_type != GetStreamMimeType(still)) {
    return absl::InvalidArgumentError(
        "Metadata specified still mime type does not match actual still mime "
        "type");
  }

  if (image_info.video_mime_type == MimeType::kUnknownMimeType) {
    return absl::InvalidArgumentError("Invalid video mime type");
  }

  if (image_info.video_mime_type != GetStreamMimeType(video)) {
    return absl::InvalidArgumentError(
        "Metadata specified video mime type does not match actual video mime "
        "type");
  }

  return absl::OkStatus();
}

}  // namespace

Demuxer::Demuxer() = default;

absl::Status Demuxer::Init(const absl::string_view motion_photo) {
  motion_photo_ = std::string(motion_photo);
  image_info_ = std::make_unique<ImageInfo>();

  std::unique_ptr<IXmpIOHelper> xmp_io_helper = GetXmpIOHelper(motion_photo_);

  if (!xmp_io_helper) {
    return absl::InvalidArgumentError("Failed to parse file as jpeg or heic");
  }

  std::unique_ptr<xmlDoc, LibXmlDeleter> xml_doc =
      xmp_io_helper->GetXmp(motion_photo_);

  if (!xml_doc) {
    return absl::InvalidArgumentError("Failed to find and parse xmp data");
  }

  RETURN_IF_ERROR(GetImageInfo(*xml_doc, image_info_.get()));
  RETURN_IF_ERROR(ValidateImageInfo(*image_info_, GetStillStringView(),
                                    GetVideoStringView(), motion_photo_));

  return absl::OkStatus();
}

absl::Status Demuxer::GetInfo(ImageInfo *image_info) {
  if (!image_info) {
    return kOutPtrIsNullError;
  }

  if (!image_info_) {
    return kDemuxerNotInitializedError;
  }

  *image_info = *image_info_;
  return absl::OkStatus();
}

absl::Status Demuxer::GetStill(std::string *still) {
  if (!still) {
    return kOutPtrIsNullError;
  }

  if (!image_info_) {
    return kDemuxerNotInitializedError;
  }

  *still = std::string(GetStillStringView());
  return absl::OkStatus();
}

absl::Status Demuxer::GetVideo(std::string *video) {
  if (!video) {
    return kOutPtrIsNullError;
  }

  if (!image_info_) {
    return kDemuxerNotInitializedError;
  }

  *video = std::string(GetVideoStringView());
  return absl::OkStatus();
}

absl::string_view Demuxer::GetStillStringView() {
  if (!image_info_) {
    return "";
  }

  return absl::string_view(motion_photo_.data(),
                           motion_photo_.length() - image_info_->video_length -
                               image_info_->still_padding);
}

absl::string_view Demuxer::GetVideoStringView() {
  if (!image_info_) {
    return "";
  }

  return absl::string_view(
      motion_photo_.data() + motion_photo_.length() - image_info_->video_length,
      image_info_->video_length);
}

}  // namespace libmphoto
