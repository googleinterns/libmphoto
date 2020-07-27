load("@rules_cc//cc:defs.bzl", "cc_test")

cc_test(
    name = "tests",
    srcs = [
        "generic_remuxing_test.cc",
        "jpeg_microvideo_remuxing_test.cc",
        "jpeg_motion_photo_remuxing_test.cc",
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
