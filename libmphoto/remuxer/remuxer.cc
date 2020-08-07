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

#include "libmphoto/remuxer/remuxer.h"

#include <sstream>

#include "absl/base/internal/endian.h"
#include "libmphoto/common/xmp_field_paths.h"
#include "libmphoto/common/xml/xml_utils.h"
#include "libmphoto/common/macros.h"
#include "libmphoto/common/stream_parser.h"

namespace libmphoto {

namespace {

constexpr char kDefaultXmp[] =
    "<x:xmpmeta\n"
    "  xmlns:x=\"adobe:ns:meta/\"\n"
    "  x:xmptk=\"Adobe XMP Core 5.1.0-jc003\">\n"
    "  <rdf:RDF\n"
    "    xmlns:rdf=\"http://www.w3.org/1999/02/22-rdf-syntax-ns#\">\n"
    "  </rdf:RDF>\n"
    "</x:xmpmeta>";

constexpr char kXmpRootXPath[] = "/x:xmpmeta/rdf:RDF[1]";

constexpr char kDefaultXmpMotionPhotoItem[] =
    "<rdf:RDF\n"
    "  xmlns:rdf=\"http://www.w3.org/1999/02/22-rdf-syntax-ns#\">\n"
    "    <rdf:Description\n"
    "      rdf:about=\"\"\n"
    "      xmlns:Camera=\"http://ns.google.com/photos/1.0/camera/\"\n"
    "      xmlns:Container=\"http://ns.google.com/photos/1.0/container/\"\n"
    "      xmlns:Item=\"http://ns.google.com/photos/1.0/container/item/\"\n"
    "      Camera:MotionPhoto=\"1\"\n"
    "      Camera:MotionPhotoVersion=\"1\"\n"
    "      Camera:MotionPhotoPresentationTimestampUs=\"0\"\n"
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
    "              Item:Length=\"0\"/>\n"
    "          </rdf:li>\n"
    "        </rdf:Seq>\n"
    "      </Container:Directory>\n"
    "    </rdf:Description>\n"
    "</rdf:RDF>\n";

absl::Status MergeXmpItemIntoXmlDoc(const std::string &xmp_item,
                                    xmlDoc *xml_doc) {
  auto xpath_context = GetXPathContext(kNamespaces, xml_doc);
  if (!xpath_context) {
    return kFailedXPathCreationError;
  }

  xmlNode *xmp_root;
  RETURN_IF_ERROR(GetXmlNode(kXmpRootXPath, *xpath_context, &xmp_root));

  std::unique_ptr<xmlDoc, LibXmlDeleter> new_xmp_item_doc(
      xmlReadMemory(xmp_item.data(), xmp_item.length(), ".xml", nullptr, 0));

  // Copy is owned by xml_doc.
  xmlNode *new_xmp_item_node =
      xmlDocCopyNode(xmlDocGetRootElement(new_xmp_item_doc.get()), xml_doc, 1);

  if (!new_xmp_item_node) {
    return absl::InvalidArgumentError("Failed to create new xmp item node");
  }

  if (!xmlAddChildList(xmp_root, new_xmp_item_node->children)) {
    return absl::InternalError("Failed to add child xmp item");
  }

  return absl::OkStatus();
}

std::unique_ptr<xmlDoc, LibXmlDeleter> GetDefaultXmp() {
  std::unique_ptr<xmlDoc, LibXmlDeleter> xml_doc(
      xmlReadMemory(kDefaultXmp, strlen(kDefaultXmp), ".xml", nullptr, 0));

  if (!MergeXmpItemIntoXmlDoc(kDefaultXmpMotionPhotoItem, xml_doc.get()).ok()) {
    return nullptr;
  }

  return xml_doc;
}

constexpr char mpvd_box_name[] = "mpvd";

absl::Status GetHeicStillPadding(const int video_length,
                                 std::string *still_padding) {
  std::stringstream ss;
  uint32_t large_signal = 1;
  char large_signal_big_endian[4];

  absl::big_endian::Store32(large_signal_big_endian, large_signal);
  ss.write(large_signal_big_endian, sizeof(large_signal_big_endian));

  ss.write(mpvd_box_name, sizeof(mpvd_box_name));

  char mpvd_box_length_big_endian[8];
  uint64_t mpvd_box_length = sizeof(large_signal_big_endian) +
                             sizeof(mpvd_box_name) +
                             sizeof(mpvd_box_length_big_endian) + video_length;

  absl::big_endian::Store64(mpvd_box_length_big_endian, mpvd_box_length);

  ss.write(mpvd_box_length_big_endian, sizeof(mpvd_box_length_big_endian));

  *still_padding = ss.str();
  return absl::OkStatus();
}

}  // namespace

Remuxer::Remuxer() = default;

absl::Status Remuxer::SetStill(const absl::string_view still,
                               int presentation_timestamp_us) {
  still_ = std::string(still);
  presentation_timestamp_us_ = presentation_timestamp_us;
  xmp_io_helper_ = GetXmpIOHelper(still_);

  if (!xmp_io_helper_) {
    return absl::InvalidArgumentError(
        "Still cannot be parsed as a jpeg or heic");
  }

  return absl::OkStatus();
}

absl::Status Remuxer::SetVideo(const absl::string_view video) {
  if (GetStreamMimeType(video) != MimeType::kVideoMp4) {
    return absl::InvalidArgumentError("Video cannot be parsed as an mp4");
  }
  video_ = std::string(video);

  return absl::OkStatus();
}

absl::Status Remuxer::Finalize(std::string *motion_photo) {
  if (still_.empty() || video_.empty()) {
    return absl::FailedPreconditionError("Still or video not set");
  }

  RETURN_IF_ERROR(GenerateStillPadding());

  // Get the current xmp to edit.
  std::unique_ptr<xmlDoc, LibXmlDeleter> xml_doc =
      xmp_io_helper_->GetXmp(still_);
  if (!xml_doc) {
    xml_doc = GetDefaultXmp();
  }

  auto xpath_context = GetXPathContext(kNamespaces, xml_doc.get());
  if (!xpath_context) {
    return kFailedXPathCreationError;
  }

  // If the still has Microvideo metadata edit it, otherwise edit Motion Photo
  // metadata.
  MPhotoFormat format = GetMPhotoFormat(*xpath_context);

  if (format == MPhotoFormat::kMicrovideo) {
    RETURN_IF_ERROR(UpdateXmpMicrovideo(xpath_context.get()));
  } else {
    if (format == MPhotoFormat::kNone) {
      RETURN_IF_ERROR(
          MergeXmpItemIntoXmlDoc(kDefaultXmpMotionPhotoItem, xml_doc.get()));
    }
    RETURN_IF_ERROR(UpdateXmpMotionPhoto(xpath_context.get()));
  }

  std::string updated_still;
  RETURN_IF_ERROR(xmp_io_helper_->SetXmp(*xml_doc, still_, &updated_still));

  *motion_photo = updated_still + still_padding_ + video_;
  return absl::OkStatus();
}

absl::Status Remuxer::UpdateXmpMotionPhoto(xmlXPathContext *xpath_context) {
  RETURN_IF_ERROR(SetXmlAttributeValue(kMotionPhotoXPath, "1", xpath_context));
  RETURN_IF_ERROR(
      SetXmlAttributeValue(kMotionPhotoVersionXPath, "1", xpath_context));
  RETURN_IF_ERROR(SetXmlAttributeValue(
      kMotionPhotoPresentationTimestampUsXPath,
      std::to_string(presentation_timestamp_us_), xpath_context));
  RETURN_IF_ERROR(SetXmlAttributeValue(
      kImageMimeTypeXPath, kMimeTypeToString.at(xmp_io_helper_->GetMimeType()),
      xpath_context));
  RETURN_IF_ERROR(SetXmlAttributeValue(
      kVideoMimeTypeXPath, kMimeTypeToString.at(MimeType::kVideoMp4),
      xpath_context));
  RETURN_IF_ERROR(SetXmlAttributeValue(
      kVideoLengthXPath, std::to_string(video_.length()), xpath_context));
  if (still_padding_.length() > 0) {
    RETURN_IF_ERROR(SetXmlAttributeValue(
        kStillPaddingXPath, std::to_string(still_padding_.length()),
        xpath_context));
  }

  return absl::OkStatus();
}

absl::Status Remuxer::UpdateXmpMicrovideo(xmlXPathContext *xpath_context) {
  RETURN_IF_ERROR(SetXmlAttributeValue(kMicrovideoXPath, "1", xpath_context));
  RETURN_IF_ERROR(
      SetXmlAttributeValue(kMicrovideoVersionXPath, "1", xpath_context));
  RETURN_IF_ERROR(SetXmlAttributeValue(
      kMicrovideoPresentationTimestampUsXPath,
      std::to_string(presentation_timestamp_us_), xpath_context));
  RETURN_IF_ERROR(SetXmlAttributeValue(
      kMicrovideoOffsetXPath, std::to_string(video_.length()), xpath_context));

  return absl::OkStatus();
}

absl::Status Remuxer::GenerateStillPadding() {
  if (xmp_io_helper_->GetMimeType() == MimeType::kImageJpeg) {
    // Jpeg Motion Photos/Microvideos do not have any paddig around still.
    still_padding_ = "";
    return absl::OkStatus();
  }

  if (xmp_io_helper_->GetMimeType() == MimeType::kImageHeic) {
    return GetHeicStillPadding(video_.length(), &still_padding_);
  }

  return absl::InternalError("Invalid motion photo mime type");
}

}  // namespace libmphoto
