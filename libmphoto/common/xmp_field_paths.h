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

#ifndef LIBMPHOTO_COMMON_XMP_FIELD_PATHS_H_
#define LIBMPHOTO_COMMON_XMP_FIELD_PATHS_H_

#include <vector>
#include <string>
#include <utility>

namespace libmphoto {

const std::vector<std::pair<const std::string, const std::string>> kNamespaces =
    {{"x", "adobe:ns:meta/"},
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

}  // namespace libmphoto

#endif  // LIBMPHOTO_COMMON_XMP_FIELD_PATHS_H_
