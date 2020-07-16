cc_library(
    name = "demuxer",
    srcs = [
        "demuxer.cc"
    ],
    hdrs = [
        "demuxer.h",
        "image_info.h",
        "libxml_deleter.h"
    ],
    deps = [
        "//libmphoto/common",
        "@absl//absl/status",
        "@libxml//:libxml"
    ],
    copts = ["-std=c++14"],
    visibility = ["//visibility:public"]
)
