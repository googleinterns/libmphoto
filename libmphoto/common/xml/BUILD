load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "xml",
    srcs = [
        "xml_utils.cc",
    ],
    hdrs = [
        "libxml_deleter.h",
        "xml_utils.h",
    ],
    visibility = [
        "//libmphoto/common:__pkg__",
        "//libmphoto/common/xmp_io:__pkg__",
    ],
    deps = [
        "@absl//absl/status",
        "@libxml",
    ],
)
