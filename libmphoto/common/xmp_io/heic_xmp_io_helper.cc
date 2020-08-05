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

#include "libmphoto/common/xmp_io/heic_xmp_io_helper.h"

#include <vector>

#include "libxml/parser.h"
#include "libheif/heif.h"
#include "libheif/heif_api_structs.h"
#include "libheif/error.h"
#include "libmphoto/common/macros.h"
#include "libmphoto/common/xmp_io/libheif_deleter.h"

namespace libmphoto {

namespace {

// As per the ISO 23008-12:2017 spec, xmp is stored with the metadata type
// "mime" and content type "application/rdf+xml".
constexpr char kMetadataTypeXmp[] = "mime";
constexpr char kContentTypeXmp[] = "application/rdf+xml";

}  // namespace

std::unique_ptr<xmlDoc, LibXmlDeleter> HeicXmpIOHelper::GetXmp(
    const std::string &image) {
  heif_error error;

  std::unique_ptr<heif_context, LibHeifDeleter> context(heif_context_alloc());
  error = heif_context_read_from_memory_without_copy(
      context.get(), image.data(), image.length(), nullptr);
  if (error.code != heif_error_Ok) {
    return nullptr;
  }

  heif_image_handle *handle_ptr;
  error = heif_context_get_primary_image_handle(context.get(), &handle_ptr);
  std::unique_ptr<heif_image_handle, LibHeifDeleter> handle(handle_ptr);
  if (error.code != heif_error_Ok) {
    return nullptr;
  }

  // Get the metadata id for the xmp section by finding all blocks that have
  // the xmp type and then filtering to the block id with the correct
  // content type.
  int block_count = heif_image_handle_get_number_of_metadata_blocks(
      handle.get(), kMetadataTypeXmp);
  std::vector<heif_item_id> metadata_ids(block_count);

  heif_image_handle_get_list_of_metadata_block_IDs(
      handle.get(), kMetadataTypeXmp, metadata_ids.data(), block_count);

  auto xmp_metadata_block_id =
      find_if(metadata_ids.begin(), metadata_ids.end(),
              [handle_ptr](const heif_item_id &id) {
                return !strcmp(
                    heif_image_handle_get_metadata_content_type(handle_ptr, id),
                    kContentTypeXmp);
              });
  if (xmp_metadata_block_id == metadata_ids.end()) {
    return nullptr;
  }

  // Extract the metadata from the found id.
  size_t xmp_size =
      heif_image_handle_get_metadata_size(handle.get(), *xmp_metadata_block_id);
  std::vector<char> xmp(xmp_size);
  error = heif_image_handle_get_metadata(handle.get(), *xmp_metadata_block_id,
                                         xmp.data());
  if (error.code != heif_error_Ok) {
    return nullptr;
  }

  return std::unique_ptr<xmlDoc, LibXmlDeleter>(
      xmlReadMemory(xmp.data(), xmp_size, ".xml", nullptr, 0));
}

absl::Status HeicXmpIOHelper::SetXmp(const xmlDoc &xml_doc,
                                     const std::string &image,
                                     std::string *updated_image) {
  return absl::UnimplementedError("");
}

MimeType HeicXmpIOHelper::GetMimeType() { return MimeType::kImageHeic; }

}  // namespace libmphoto
