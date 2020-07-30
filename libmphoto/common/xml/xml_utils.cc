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

#include "libmphoto/common/xml/xml_utils.h"

#include <memory>

#include "libxml/parser.h"
#include "libxml/xpathInternals.h"
#include "libmphoto/common/xml/libxml_deleter.h"
#include "libmphoto/common/macros.h"

namespace libmphoto {

namespace {

absl::Status RegisterNamespaces(
    const std::vector<std::pair<const std::string, const std::string>>
        namespaces,
    xmlXPathContext *xpath_context) {
  for (const auto &ns : namespaces) {
    if (xmlXPathRegisterNs(
            xpath_context, reinterpret_cast<const xmlChar *>(ns.first.c_str()),
            reinterpret_cast<const xmlChar *>(ns.second.c_str()))) {
      return absl::InternalError("Failed to register namespace: " + ns.first +
                                 "-" + ns.second);
    }
  }

  return absl::OkStatus();
}

}  // namespace

absl::Status GetXmlNode(const std::string &xpath,
                        const xmlXPathContext &xpath_context,
                        xmlNode **xml_node) {
  std::unique_ptr<xmlXPathObject, LibXmlDeleter> xpath_object(
      xmlXPathEvalExpression(reinterpret_cast<const xmlChar *>(xpath.c_str()),
                             const_cast<xmlXPathContext *>(&xpath_context)));

  if (!xpath_object || !xpath_object->nodesetval ||
      !xpath_object->nodesetval->nodeTab) {
    return absl::NotFoundError("No node found for xpath: " + xpath);
  }

  *xml_node = xpath_object->nodesetval->nodeTab[0];
  return absl::OkStatus();
}

absl::Status GetXmlAttributeValue(const std::string &xpath,
                                  const xmlXPathContext &xpath_context,
                                  std::string *result) {
  xmlNode *xml_node;
  RETURN_IF_ERROR(GetXmlNode(xpath, xpath_context, &xml_node));
  if (!xml_node->children || !xml_node->children->content) {
    return absl::NotFoundError("No content found for xpath: " + xpath);
  }

  // The value is the content of the first child.
  *result = std::string(reinterpret_cast<char *>(xml_node->children->content));
  return absl::OkStatus();
}

absl::Status SetXmlAttributeValue(const std::string &xpath,
                                  const std::string &value,
                                  xmlXPathContext *xpath_context) {
  std::unique_ptr<xmlXPathObject, LibXmlDeleter> xpath_object(
      xmlXPathEvalExpression(reinterpret_cast<const xmlChar *>(xpath.c_str()),
                             xpath_context));

  if (!xpath_object || !xpath_object->nodesetval ||
      !xpath_object->nodesetval->nodeTab ||
      !xpath_object->nodesetval->nodeTab[0]->children ||
      !xpath_object->nodesetval->nodeTab[0]->children->content) {
    return absl::NotFoundError("No value found for xpath: " + xpath);
  }

  xmlNodeSetContent(xpath_object->nodesetval->nodeTab[0],
                    reinterpret_cast<const xmlChar *>(value.c_str()));

  return absl::OkStatus();
}

std::unique_ptr<xmlXPathContext, LibXmlDeleter> GetXPathContext(
    const std::vector<std::pair<const std::string, const std::string>>
        namespaces,
    xmlDoc *xml_doc) {
  std::unique_ptr<xmlXPathContext, LibXmlDeleter> xpath_context(
      xmlXPathNewContext(xml_doc));

  if (!xpath_context) {
    return nullptr;
  }

  if (!RegisterNamespaces(namespaces, xpath_context.get()).ok()) {
    return nullptr;
  }

  return xpath_context;
}

}  // namespace libmphoto
