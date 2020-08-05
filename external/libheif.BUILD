load("@rules_cc//cc:defs.bzl", "cc_library")

libheif_hdrs = glob(
    [
        "libheif/*.h",
    ],
    exclude = [
        "libheif/heif_decoder_aom.h",
        "libheif/heif_encoder_aom.h",
        "libheif/heif_emscripten.h",
        "libheif/heif_encoder_x265.cc",
    ],
)

libheif_srcs = glob(
    [
        "libheif/*.cc",
    ],
    exclude = [
        "libheif/heif_decoder_aom.cc",
        "libheif/heif_encoder_aom.cc",
        "libheif/heif_encoder_x265.cc",
        "libheif/*fuzzer.cc",
    ],
)

cc_library(
    name = "libheif",
    srcs = libheif_srcs,
    hdrs = libheif_hdrs,
    includes = ["."],
    linkstatic = True,
    local_defines = [
        "HAVE_UNISTD_H",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":libheif_version",
        "@libde265",
    ],
)

cc_library(
    name = "libheif_version",
    hdrs = ["libheif/heif_version.h"],
    data = [":libheif_heif_version_h"],
    includes = ["."],
)

genrule(
    name = "libheif_heif_version_h",
    srcs = [
        "configure.ac",
        "libheif/heif_version.h.in",
    ],
    outs = ["libheif/heif_version.h"],
    cmd = """
    major_version=$$(grep PROJECT_VERSION_MAJOR= $(location configure.ac) | awk 'BEGIN { FS = "=" } { print $$2 }'); \\
    minor_version=$$(grep PROJECT_VERSION_MINOR= $(location configure.ac) | awk 'BEGIN { FS = "=" } { print $$2 }'); \\
    patch_version=$$(grep PROJECT_VERSION_PATCH= $(location configure.ac) | awk 'BEGIN { FS = "=" } { print $$2 }'); \\
    tweak_version=$$(grep PROJECT_VERSION_TWEAK= $(location configure.ac) | awk 'BEGIN { FS = "=" } { print $$2 }'); \\
    sed $(location libheif/heif_version.h.in) -e "s/@PROJECT_VERSION_MAJOR@/$$major_version/g" -e "s/@PROJECT_VERSION_MINOR@/$$minor_version/g" \\
       -e "s/@PROJECT_VERSION_PATCH@/$$patch_version/g" -e "s/@PROJECT_VERSION_TWEAK@/$$tweak_version/g" > $(location libheif/heif_version.h);
    """,
)
