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

#include "libmphoto/common/xmp_field_paths.h"
#include "libmphoto/common/xml/xml_utils.h"
#include "libmphoto/common/macros.h"

namespace libmphoto {

namespace {

constexpr char kDefaultXmp[] =
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
    "  </rdf:RDF>\n"
    "</x:xmpmeta>";

std::unique_ptr<xmlDoc, LibXmlDeleter> GetDefaultXmp() {
  return std::unique_ptr<xmlDoc, LibXmlDeleter>(
      xmlReadMemory(kDefaultXmp, strlen(kDefaultXmp), ".xml", nullptr, 0));
}

}  // namespace

Remuxer::Remuxer() = default;

absl::Status Remuxer::SetStill(const absl::string_view still) {
  still_ = std::string(still);
  xmp_io_helper_ = GetXmpIOHelper(still_);

  if (!xmp_io_helper_) {
    return absl::InvalidArgumentError(
        "Still cannot be parsed as a jpeg or heic");
  }

  return absl::OkStatus();
}

absl::Status Remuxer::SetVideo(const absl::string_view video) {
  video_ = std::string(video);
  // Todo(pinheirojamie) validate the video is an mp4.
  return absl::OkStatus();
}

absl::Status Remuxer::Finalize(std::string *motion_photo) {
  // Get the current xmp to edit.
  std::unique_ptr<xmlDoc, LibXmlDeleter> xml_doc =
      xmp_io_helper_->GetXmp(still_);
  if (!xml_doc) {
    xml_doc = GetDefaultXmp();
  }

  std::unique_ptr<xmlXPathContext, LibXmlDeleter> xpath_context(
      xmlXPathNewContext(xml_doc.get()));
  if (!xpath_context) {
    return absl::InternalError("Failed to create xpath context");
  }

  RETURN_IF_ERROR(RegisterNamespaces(kNamespaces, xpath_context.get()));

  // If the still has Microvideo metadata edit it, otherwise edit Motion Photo
  // metadata.
  std::string value;
  if (GetXmlAttributeValue(kMicrovideoXPath, *xpath_context, &value).ok()) {
    RETURN_IF_ERROR(UpdateXmpMicrovideo(xpath_context.get(), xml_doc.get()));
  } else {
    RETURN_IF_ERROR(UpdateXmpMotionPhoto(xpath_context.get(), xml_doc.get()));
  }

  std::string updated_still;
  RETURN_IF_ERROR(xmp_io_helper_->SetXmp(*xml_doc, still_, &updated_still));

  *motion_photo = updated_still + video_;
  return absl::OkStatus();
}

absl::Status Remuxer::UpdateXmpMotionPhoto(xmlXPathContext *xpath_context,
                                           xmlDoc *xml_doc) {
  RETURN_IF_ERROR(SetXmlAttributeValue(kMotionPhotoXPath, "1", xpath_context));
  RETURN_IF_ERROR(
      SetXmlAttributeValue(kMotionPhotoVersionXPath, "1", xpath_context));
  // Todo(pinheirojamie) expose editing this through the api.
  RETURN_IF_ERROR(SetXmlAttributeValue(kMotionPhotoPresentationTimestampUsXPath,
                                       "0", xpath_context));
  RETURN_IF_ERROR(SetXmlAttributeValue(
      kImageMimeTypeXPath, kMimeTypeToString.at(xmp_io_helper_->GetMimeType()),
      xpath_context));
  RETURN_IF_ERROR(SetXmlAttributeValue(
      kVideoMimeTypeXPath, kMimeTypeToString.at(MimeType::kVideoMp4),
      xpath_context));
  RETURN_IF_ERROR(SetXmlAttributeValue(
      kVideoLengthXPath, std::to_string(video_.length()), xpath_context));

  return absl::OkStatus();
}

absl::Status Remuxer::UpdateXmpMicrovideo(xmlXPathContext *xpath_context,
                                          xmlDoc *xml_doc) {
  return absl::UnimplementedError("");
}

}  // namespace libmphoto
