load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "common",
    srcs = [
        "stream_parser.cc",
    ],
    hdrs = [
        "macros.h",
        "mime_type.h",
        "stream_parser.h",
        "xmp_field_paths.h",
    ],
    visibility = [
        "//libmphoto/demuxer:__pkg__",
        "//libmphoto/remuxer:__pkg__",
    ],
    deps = [
        "@absl//absl/status",
    ],
)

cc_library(
    name = "xmp",
    srcs = [
        "xml/xml_utils.cc",
        "xmp_io/heic_xmp_io_helper.cc",
        "xmp_io/jpeg_xmp_io_helper.cc",
        "xmp_io/xmp_io_helper.cc",
    ],
    hdrs = [
        "xml/libxml_deleter.h",
        "xml/xml_utils.h",
        "xmp_io/heic_xmp_io_helper.h",
        "xmp_io/jpeg_xmp_io_helper.h",
        "xmp_io/libheif_deleter.h",
        "xmp_io/xmp_io_helper.h",
    ],
    visibility = [
        "//libmphoto/demuxer:__pkg__",
        "//libmphoto/remuxer:__pkg__",
    ],
    deps = [
        "//libmphoto/common",
        "@absl//absl/status",
        "@libheif",
        "@libxml",
        "@xmpmeta",
    ],
)
