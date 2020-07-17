cc_test(
    name = "tests",
    srcs = [
        "information_extraction_test.cc",
        "still_demuxing_test.cc",
        "video_demuxing_test.cc",
    ],
    data = [
        "//sample_data:microvideo.jpg",
        "//sample_data:microvideo_still.jpg",
        "//sample_data:microvideo_video.mp4",
        "//sample_data:motionphoto.jpg",
        "//sample_data:motionphoto_still.jpg",
        "//sample_data:motionphoto_video.mp4",
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
