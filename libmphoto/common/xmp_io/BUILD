load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "xmp_io",
    srcs = [
        "heic_xmp_io_helper.cc",
        "jpeg_xmp_io_helper.cc",
        "xmp_io_helper.cc",
    ],
    hdrs = [
        "heic_xmp_io_helper.h",
        "jpeg_xmp_io_helper.h",
        "xmp_io_helper.h",
    ],
    visibility = [
        "//libmphoto/common:__pkg__",
    ],
    deps = [
        "//libmphoto/common:mime_type",
        "//libmphoto/common/xml",
        "@absl//absl/status",
        "@libxml",
        "@xmpmeta",
    ],
)
