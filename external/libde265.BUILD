load("@rules_cc//cc:defs.bzl", "cc_library")

libde265_hdrs = glob([
    "libde265/**/*.h",
    "extra/libde265/*.h",
])

libde265_srcs = glob([
    "libde265/**/*.{h,c,cc}",
    "extra/libde265/*.h",
])

cc_library(
    name = "libde265",
    srcs = libde265_srcs,
    hdrs = libde265_hdrs,
    copts = ["-Wno-non-virtual-dtor"],
    includes = ["."],
    visibility = ["//visibility:public"],
    deps = [":libde265_version"],
)

cc_library(
    name = "libde265_version",
    hdrs = ["libde265/de265-version.h"],
    data = [":libde265_de265_version_h"],
    includes = ["."],
)

genrule(
    name = "libde265_de265_version_h",
    srcs = [
        "configure.ac",
        "libde265/de265-version.h.in",
    ],
    outs = ["libde265/de265-version.h"],
    cmd = """
    numeric_version=1; \\
    sed $(location libde265/de265-version.h.in) -e "s/@NUMERIC_VERSION@/$$numeric_version/g" -e "s/@PACKAGE_VERSION@/$$numeric_version/g" > $(location libde265/de265-version.h);
    """,
)
