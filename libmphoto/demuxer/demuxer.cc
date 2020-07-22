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
#include "libxml/parser.h"
#include "libxml/tree.h"
#include "libxml/xpath.h"
#include "libxml/xpathInternals.h"
#include "libmphoto/common/macros.h"
#include "libmphoto/common/libxml_deleter.h"
#include "libmphoto/common/xmp_io_helper.h"
#include "libmphoto/common/jpeg_xmp_io_helper.h"
#include "libmphoto/common/heic_xmp_io_helper.h"

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

const std::pair<const std::string, const std::string> kNamespaces[] = {
    {"x", "adobe:ns:meta/"},
    {"rdf", "http://www.w3.org/1999/02/22-rdf-syntax-ns#"},
    {"Camera", "http://ns.google.com/photos/1.0/camera/"},
    {"Container", "http://ns.google.com/photos/1.0/container/"},
    {"Item", "http://ns.google.com/photos/1.0/container/item/"},
    {"GCamera", "http://ns.google.com/photos/1.0/camera/"}};

// Motion Photo Spec metadata xpaths.
constexpr char kMotionPhotoXPath[] =
    "/x:xmpmeta/rdf:RDF/rdf:Description/@Camera:MotionPhoto";
constexpr char kMotionPhotoVersionXPath[] =
    "/x:xmpmeta/rdf:RDF/rdf:Description/@Camera:MotionPhotoVersion";
constexpr char kMotionPhotoPresentationTimestampUsXPath[] =
    "/x:xmpmeta/rdf:RDF/rdf:Description/"
    "@Camera:MotionPhotoPresentationTimestampUs";
constexpr char kImageMimeTypeXPath[] =
    "/x:xmpmeta/rdf:RDF/rdf:Description/Container:Directory/rdf:Seq/rdf:li[1]/"
    "Container:Item/@Item:Mime";
constexpr char kVideoMimeTypeXPath[] =
    "/x:xmpmeta/rdf:RDF/rdf:Description/Container:Directory/rdf:Seq/rdf:li[2]/"
    "Container:Item/@Item:Mime";
constexpr char kVideoLengthXPath[] =
    "/x:xmpmeta/rdf:RDF/rdf:Description/Container:Directory/rdf:Seq/rdf:li[2]/"
    "Container:Item/@Item:Length";

// Microvideo (deprecated) metadata xpaths.
constexpr char kMicrovideoXPath[] =
    "/x:xmpmeta/rdf:RDF/rdf:Description/@GCamera:MicroVideo";
constexpr char kMicrovideoVersionXPath[] =
    "/x:xmpmeta/rdf:RDF/rdf:Description/@GCamera:MicroVideoVersion";
constexpr char kMicrovideoOffsetXPath[] =
    "/x:xmpmeta/rdf:RDF/rdf:Description/@GCamera:MicroVideoOffset";
constexpr char kMicrovideoPresentationTimestampUsXPath[] =
    "/x:xmpmeta/rdf:RDF/rdf:Description/"
    "@GCamera:MicroVideoPresentationTimestampUs";
constexpr MimeType kMicrovideoImageMimeType = MimeType::kImageJpeg;
constexpr MimeType kMicrovideoVideoMimeType = MimeType::kVideoMp4;

absl::Status GetXmlAttributeValue(const std::string &xpath,
                                  const xmlXPathContext &xpath_context,
                                  std::string *result) {
  std::unique_ptr<xmlXPathObject, LibXmlDeleter> xpath_object(
      xmlXPathEvalExpression(reinterpret_cast<const xmlChar *>(xpath.c_str()),
                             const_cast<xmlXPathContext *>(&xpath_context)));

  if (!xpath_object || !xpath_object->nodesetval ||
      !xpath_object->nodesetval->nodeTab ||
      !xpath_object->nodesetval->nodeTab[0]->children ||
      !xpath_object->nodesetval->nodeTab[0]->children->content) {
    return absl::NotFoundError("No value found for xpath: " + xpath);
  }

  // The value is the content of the first child of the result.
  *result = std::string(reinterpret_cast<char *>(
      xpath_object->nodesetval->nodeTab[0]->children->content));

  return absl::OkStatus();
}

const std::map<std::string, MimeType> kStringToMimeType = {
    {"image/jpeg", MimeType::kImageJpeg},
    {"image/jpg", MimeType::kImageJpeg},
    {"image/heic", MimeType::kImageHeic},
    {"video/mp4", MimeType::kVideoMp4}};

MimeType GetMimeType(const std::string &input) {
  // Todo(pinheirojamie) do signature checks to validate mime types.
  std::string lower_case_input = absl::AsciiStrToLower(input);

  if (kStringToMimeType.find(lower_case_input) != kStringToMimeType.end()) {
    return kStringToMimeType.at(lower_case_input);
  }

  return MimeType::kUnknownMimeType;
}

absl::Status GetImageInfoFromMotionPhoto(const xmlDoc &xml_doc,
                                         const xmlXPathContext &xpath_context,
                                         ImageInfo *image_info) {
  std::string value;

  RETURN_IF_ERROR(
      GetXmlAttributeValue(kMotionPhotoXPath, xpath_context, &value))
  if (!absl::SimpleAtoi(value, &image_info->motion_photo)) {
    return kIncorrectTypeError;
  }

  RETURN_IF_ERROR(
      GetXmlAttributeValue(kMotionPhotoVersionXPath, xpath_context, &value))
  if (!absl::SimpleAtoi(value, &image_info->motion_photo_version)) {
    return kIncorrectTypeError;
  }

  RETURN_IF_ERROR(GetXmlAttributeValue(kMotionPhotoPresentationTimestampUsXPath,
                                       xpath_context, &value))
  if (!absl::SimpleAtoi(value,
                        &image_info->motion_photo_presentation_timestamp_us)) {
    return kIncorrectTypeError;
  }

  RETURN_IF_ERROR(
      GetXmlAttributeValue(kImageMimeTypeXPath, xpath_context, &value))
  image_info->image_mime_type = GetMimeType(value);

  RETURN_IF_ERROR(
      GetXmlAttributeValue(kVideoMimeTypeXPath, xpath_context, &value))
  image_info->video_mime_type = GetMimeType(value);

  RETURN_IF_ERROR(
      GetXmlAttributeValue(kVideoLengthXPath, xpath_context, &value))
  if (!absl::SimpleAtoi(value, &image_info->video_length)) {
    return kIncorrectTypeError;
  }

  return absl::OkStatus();
}

absl::Status GetImageInfoFromMicrovideo(const xmlDoc &xml_doc,
                                        const xmlXPathContext &xpath_context,
                                        ImageInfo *image_info) {
  std::string value;

  RETURN_IF_ERROR(GetXmlAttributeValue(kMicrovideoXPath, xpath_context, &value))
  if (!absl::SimpleAtoi(value, &image_info->motion_photo)) {
    return kIncorrectTypeError;
  }

  RETURN_IF_ERROR(
      GetXmlAttributeValue(kMicrovideoVersionXPath, xpath_context, &value))
  if (!absl::SimpleAtoi(value, &image_info->motion_photo_version)) {
    return kIncorrectTypeError;
  }

  RETURN_IF_ERROR(GetXmlAttributeValue(kMicrovideoPresentationTimestampUsXPath,
                                       xpath_context, &value))
  if (!absl::SimpleAtoi(value,
                        &image_info->motion_photo_presentation_timestamp_us)) {
    return kIncorrectTypeError;
  }

  // Image/Video Mime Type are as specified in Microvideo spec.
  image_info->image_mime_type = kMicrovideoImageMimeType;
  image_info->video_mime_type = kMicrovideoVideoMimeType;

  RETURN_IF_ERROR(
      GetXmlAttributeValue(kMicrovideoOffsetXPath, xpath_context, &value))
  if (!absl::SimpleAtoi(value, &image_info->video_length)) {
    return kIncorrectTypeError;
  }

  return absl::OkStatus();
}

absl::Status GetImageInfo(const xmlDoc &xml_doc, ImageInfo *image_info) {
  std::unique_ptr<xmlXPathContext, LibXmlDeleter> xpath_context(
      xmlXPathNewContext(const_cast<xmlDoc *>(&xml_doc)));

  if (!xpath_context) {
    return absl::InternalError("Failed to create xpath context");
  }

  // Register valid namespaces needed to parse out relevant fields.
  for (const auto &ns : kNamespaces) {
    if (xmlXPathRegisterNs(
            xpath_context.get(),
            reinterpret_cast<const xmlChar *>(ns.first.c_str()),
            reinterpret_cast<const xmlChar *>(ns.second.c_str()))) {
      return absl::InternalError("Failed to register namespace: " + ns.first +
                                 "-" + ns.second);
    }
  }

  std::string value;

  // Check if xmp has field signifying it is a Motion Photo.
  if (GetXmlAttributeValue(kMotionPhotoXPath, *xpath_context, &value).ok()) {
    return GetImageInfoFromMotionPhoto(xml_doc, *xpath_context, image_info);
  }

  // Check if xmp has field signifying it is a Microvideo.
  if (GetXmlAttributeValue(kMicrovideoXPath, *xpath_context, &value).ok()) {
    return GetImageInfoFromMicrovideo(xml_doc, *xpath_context, image_info);
  }

  // Not a Motion Photo or Microvideo.
  return kInvalidMotionPhotoError;
}

absl::Status ValidateImageInfo(const ImageInfo &image_info,
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

  if (image_info.image_mime_type == MimeType::kUnknownMimeType) {
    return absl::InvalidArgumentError("Invalid image mime type");
  }

  if (image_info.video_mime_type == MimeType::kUnknownMimeType) {
    return absl::InvalidArgumentError("Invalid video mime type");
  }

  return absl::OkStatus();
}

}  // namespace

Demuxer::Demuxer() = default;

absl::Status Demuxer::Init(const absl::string_view motion_photo) {
  motion_photo_ = std::string(motion_photo);
  image_info_ = std::make_unique<ImageInfo>();

  std::unique_ptr<IXmpIOHelper> xmp_helper = GetXmpHelper(motion_photo_);

  if (!xmp_helper) {
    return absl::InvalidArgumentError("Failed to parse file as jpeg or heic");
  }

  std::unique_ptr<xmlDoc, LibXmlDeleter> xml_doc =
      xmp_helper->GetXmp(motion_photo_);

  if (!xml_doc) {
    return absl::InvalidArgumentError("Failed to find and parse xmp data");
  }

  RETURN_IF_ERROR(GetImageInfo(*xml_doc, image_info_.get()))
  RETURN_IF_ERROR(ValidateImageInfo(*image_info_, motion_photo_));

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

  *still = motion_photo_.substr(
      0, motion_photo_.length() - image_info_->video_length);
  return absl::OkStatus();
}

absl::Status Demuxer::GetVideo(std::string *video) {
  if (!video) {
    return kOutPtrIsNullError;
  }

  if (!image_info_) {
    return kDemuxerNotInitializedError;
  }

  *video =
      motion_photo_.substr(motion_photo_.length() - image_info_->video_length);
  return absl::OkStatus();
}

}  // namespace libmphoto
