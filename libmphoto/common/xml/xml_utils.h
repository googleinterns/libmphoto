/*
 * Copyright 2020 Google LLC
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef LIBMPHOTO_COMMON_XML_XML_UTILS_H_
#define LIBMPHOTO_COMMON_XML_XML_UTILS_H_

#include <vector>
#include <utility>
#include <string>

#include "absl/status/status.h"
#include "libxml/xpath.h"

namespace libmphoto {

// Gets the value of a particular xml attribute.
absl::Status GetXmlAttributeValue(const std::string &xpath,
                                  const xmlXPathContext &xpath_context,
                                  std::string *result);

// Sets the value of a particular xml attribute.
absl::Status SetXmlAttributeValue(const std::string &xpath,
                                  const std::string &value,
                                  xmlXPathContext *xpath_context);

// Registers an array of namespaces to an xpath context.
absl::Status RegisterNamespaces(
    const std::vector<std::pair<const std::string, const std::string>>
        namespaces,
    xmlXPathContext *xpath_context);

}  // namespace libmphoto

#endif  // LIBMPHOTO_COMMON_XML_XML_UTILS_H_
