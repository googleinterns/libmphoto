load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "demuxer",
    srcs = [
        "demuxer.cc",
    ],
    hdrs = [
        "demuxer.h",
        "image_info.h",
    ],
    copts = ["-std=c++14"],
    visibility = ["//visibility:public"],
    deps = [
        "//libmphoto/common",
        "@absl//absl/status",
        "@libxml",
    ],
)
