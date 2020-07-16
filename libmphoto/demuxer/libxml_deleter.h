// Copyright 2020 Google LLC
// Author: pinheirojamie

#ifndef LIBMPHOTO_DEMUXER_LIBXML_DELETER_H_
#define LIBMPHOTO_DEMUXER_LIBXML_DELETER_H_

#include "libxml/parser.h"
#include "libxml/tree.h"
#include "libxml/xpath.h"
#include "libxml/xpathInternals.h"

namespace libmphoto {

struct LibXmlDeleter {
  void operator()(xmlDocPtr xml_doc) { xmlFreeDoc(xml_doc); }
  void operator()(xmlXPathContextPtr xpath_context) {
    xmlXPathFreeContext(xpath_context);
  }
  void operator()(xmlXPathObjectPtr xpath_object) {
    xmlXPathFreeObject(xpath_object);
  }
};

}  // namespace libmphoto

#endif  // LIBMPHOTO_DEMUXER_LIBXML_DELETER_H_
