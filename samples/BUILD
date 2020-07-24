load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")

cc_binary(
    name = "demux",
    srcs = [
        "demux.cc",
    ],
    deps = [
        ":samples",
        "//libmphoto/demuxer",
    ],
)

cc_binary(
    name = "remux",
    srcs = [
        "remux.cc",
    ],
    deps = [
        ":samples",
        "//libmphoto/remuxer",
    ],
)

cc_library(
    name = "samples",
    hdrs = [
        "macros.h",
    ],
)
