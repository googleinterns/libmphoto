cc_library(
    name = "demuxer",
    srcs = [
        "demuxer.cc",
    ],
    hdrs = [
        "demuxer.h",
        "image_info.h",
        "libxml_deleter.h",
    ],
    copts = ["-std=c++14"],
    visibility = ["//visibility:public"],
    deps = [
        "//libmphoto/common",
        "@absl//absl/status",
        "@libxml",
    ],
)
