load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

http_archive(
    name = "absl",
    strip_prefix = "abseil-cpp-23f1f9cf6d02f834d55c0595566c0d607d4c5ed8",
    urls = ["https://github.com/abseil/abseil-cpp/archive/23f1f9cf6d02f834d55c0595566c0d607d4c5ed8.zip"],
)

http_archive(
    name = "rules_cc",
    strip_prefix = "rules_cc-5cbd3dfbd1613f71ef29bbb7b10310b81e272975",
    urls = ["https://github.com/bazelbuild/rules_cc/archive/5cbd3dfbd1613f71ef29bbb7b10310b81e272975.zip"],
)

http_archive(
    name = "googletest",
    strip_prefix = "googletest-356f2d264a485db2fcc50ec1c672e0d37b6cb39b",
    urls = ["https://github.com/google/googletest/archive/356f2d264a485db2fcc50ec1c672e0d37b6cb39b.zip"],
)

http_archive(
    name = "libxml",
    build_file = "libxml.BUILD",
    strip_prefix = "libxml2-2.9.7",
    urls = [
        "https://mirror.bazel.build/xmlsoft.org/sources/libxml2-2.9.7.tar.gz",
        "http://xmlsoft.org/sources/libxml2-2.9.7.tar.gz",
    ],
)

http_archive(
    name = "xmpmeta",
    build_file = "xmpmeta.BUILD",
    strip_prefix = "xmpmeta-73671eccaef4879bb89fa98ee3e50514760f6c97",
    urls = ["https://github.com/google/xmpmeta/archive/73671eccaef4879bb89fa98ee3e50514760f6c97.zip"],
)
