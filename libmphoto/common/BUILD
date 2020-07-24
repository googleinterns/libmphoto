load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "common",
    hdrs = [
        "macros.h",
        "xmp_field_paths.h",
    ],
    visibility = [
        "//libmphoto/demuxer:__pkg__",
        "//libmphoto/remuxer:__pkg__",
    ],
    deps = [
        "//libmphoto/common:mime_type",
        "//libmphoto/common/xml",
        "//libmphoto/common/xmp_io",
        "@absl//absl/status",
        "@libxml",
        "@xmpmeta",
    ],
)

cc_library(
    name = "mime_type",
    hdrs = [
        "mime_type.h",
    ],
    visibility = ["//visibility:public"],
)
