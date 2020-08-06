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

constexpr char kEncoding[] = "UTF-8";

static struct heif_error heif_memory_writer(struct heif_context *ctx,
                                            const void *data, size_t size,
                                            void *userdata) {
  auto ss = reinterpret_cast<std::stringstream *>(userdata);

  ss->write(static_cast<const char *>(data), size);
  return heif_error();
}

// Serializes an XMP document to a string.
absl::Status SerializeXmp(const xmlDoc &xml_doc,
                          std::string *serialized_value) {
  std::ostringstream serialized_stream;
  xmlChar *value;
  int doc_size = 0;
  xmlDocDumpFormatMemoryEnc(const_cast<xmlDoc *>(&xml_doc), &value, &doc_size,
                            kEncoding, 1);

  std::unique_ptr<xmlChar, LibXmlDeleter> xml_doc_contents(value);

  if (!xml_doc_contents) {
    return absl::InvalidArgumentError("Xml failed to serialize");
  }

  const char *xml_doc_string =
      reinterpret_cast<const char *>(xml_doc_contents.get());

  // Find the index of the second "<" so we can discard the first element,
  // which is <?xml version...>, so start searching after the first "<". XMP
  // starts directly afterwards.
  const char *xmp_start_pos = strchr(&xml_doc_string[2], '<');
  if (xmp_start_pos == nullptr) {
    return absl::InvalidArgumentError("Failed to find start of xmp in xml");
  }

  const int xmp_start_idx = static_cast<int>(xmp_start_pos - xml_doc_string);

  serialized_stream.write(&xml_doc_string[xmp_start_idx],
                          doc_size - xmp_start_idx);
  *serialized_value = serialized_stream.str();
  return absl::OkStatus();
}

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
  heif_error error;

  std::unique_ptr<heif_context> context(heif_context_alloc());
  error = heif_context_read_from_memory_without_copy(
      context.get(), image.data(), image.length(), nullptr);
  if (error.code != heif_error_Ok) {
    return absl::InvalidArgumentError("Failed to create heif context");
  }

  heif_image_handle *handle;
  error = heif_context_get_primary_image_handle(context.get(), &handle);
  if (error.code != heif_error_Ok) {
    return absl::InvalidArgumentError("Failed to get primary image handle");
  }

  std::string serialized_xmp;
  RETURN_IF_ERROR(SerializeXmp(xml_doc, &serialized_xmp));

  heif_image *img;
  error = heif_decode_image(handle, &img, heif_colorspace_RGB,
                            heif_chroma_interleaved_RGB, nullptr);

  error = heif_context_add_XMP_metadata(
      context.get(), handle, serialized_xmp.data(), serialized_xmp.length());

  // https://github.com/strukturag/libheif/issues/127"
  return absl::UnimplementedError("libheif can't transmux xmp metadata");

  heif_writer writer;
  writer.writer_api_version = 1;
  writer.write = heif_memory_writer;

  std::stringstream updated_image_stream;
  error = heif_context_write(context.get(), &writer, &updated_image_stream);

  if (error.code != heif_error_Ok) {
    return absl::InternalError("Failed to write xmp back to heif");
  }

  *updated_image = updated_image_stream.str();
  return absl::OkStatus();
}

MimeType HeicXmpIOHelper::GetMimeType() { return MimeType::kImageHeic; }

}  // namespace libmphoto
