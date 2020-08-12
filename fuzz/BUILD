load("@rules_fuzzing//fuzzing:cc_deps.bzl", "cc_fuzz_test")

cc_fuzz_test(
    name = "demuxer_fuzz_test",
    srcs = ["demuxer_fuzz_test.cc"],
    corpus = [
        "sample_data/jpeg_motion_photo/motion_photo.jpeg",
        "sample_data/heic_motion_photo/motion_photo.heic",
        "sample_data/microvideo/microvideo.jpeg",
    ],
    data = [
        "//sample_data",
    ],
    deps = [
        "//libmphoto/demuxer",
    ],
)

cc_fuzz_test(
    name = "remuxer_fuzz_test",
    srcs = ["remuxer_fuzz_test.cc"],
    corpus = [
        "sample_data/jpeg/**",
        "sample_data/mp4/video.mp4",
    ],
    data = [
        "//sample_data",
    ],
    deps = [
        "//libmphoto/remuxer",
    ],
)
