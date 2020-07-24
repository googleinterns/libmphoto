load("@rules_cc//cc:defs.bzl", "cc_test")

cc_test(
    name = "tests",
    srcs = [
        "jpeg_remuxing_test.cc",
    ],
    data = [
        "//sample_data",
    ],
    linkopts = [
        "-pthread",
        "-ldl",
    ],
    deps = [
        "//libmphoto/demuxer",
        "//libmphoto/remuxer",
        "//tests/common:io_helper",
        "@googletest//:gtest",
        "@googletest//:gtest_main",
    ],
)
