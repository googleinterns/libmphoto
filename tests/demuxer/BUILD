cc_test(
    name = "tests",
    srcs = [
        "information_extraction_test.cc"
    ],
    deps = [
        "//libmphoto/demuxer",
        "@googletest//:gtest",
        "@googletest//:gtest_main",
    ],
    data = [
        "//images:microvideo.jpg",
        "//images:motionphoto.jpg"
    ],
    linkopts = ["-pthread", "-ldl"],
)
