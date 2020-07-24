load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "remuxer",
    srcs = [
        "remuxer.cc",
    ],
    hdrs = [
        "remuxer.h",
    ],
    copts = ["-std=c++14"],
    visibility = ["//visibility:public"],
    deps = [
        "//libmphoto/common",
        "@absl//absl/status",
    ],
)
