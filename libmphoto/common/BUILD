load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "common",
    srcs = [
        "heic_xmp_io_helper.cc",
        "jpeg_xmp_io_helper.cc",
        "xmp_io_helper.cc",
    ],
    hdrs = [
        "heic_xmp_io_helper.h",
        "jpeg_xmp_io_helper.h",
        "libxml_deleter.h",
        "macros.h",
        "xmp_io_helper.h",
    ],
    visibility = ["//libmphoto/demuxer:__pkg__"],
    deps = [
        "@absl//absl/status",
        "@libxml",
        "@xmpmeta",
    ],
)
