load("@rules_cc//cc:defs.bzl", "cc_test")

cc_test(
    name = "tests",
    srcs = [
        "information_extraction_test.cc",
        "still_demuxing_test.cc",
        "video_demuxing_test.cc",
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
        "//tests/common:io_helper",
        "@googletest//:gtest",
        "@googletest//:gtest_main",
    ],
)
