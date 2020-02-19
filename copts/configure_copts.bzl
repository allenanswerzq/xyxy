"""QIAN specific copts.

This file simply selects the correct options from the generated files.  To
change Abseil copts, edit QIAN/copts/copts.py
"""
load(
    "//copts:GENERATED_copts.bzl",
    "QIAN_CLANG_CL_FLAGS",
    "QIAN_CLANG_CL_TEST_FLAGS",
    "QIAN_GCC_FLAGS",
    "QIAN_GCC_TEST_FLAGS",
    "QIAN_LLVM_FLAGS",
    "QIAN_LLVM_TEST_FLAGS",
    "QIAN_MSVC_FLAGS",
    "QIAN_MSVC_LINKOPTS",
    "QIAN_MSVC_TEST_FLAGS",
    "QIAN_RANDOM_HWAES_ARM32_FLAGS",
    "QIAN_RANDOM_HWAES_ARM64_FLAGS",
    "QIAN_RANDOM_HWAES_MSVC_X64_FLAGS",
    "QIAN_RANDOM_HWAES_X64_FLAGS",
)

QIAN_DEFAULT_COPTS = select({
    "//conditions:default": QIAN_GCC_FLAGS,
})

# in absence of modules (--compiler=gcc or -c opt), cc_tests leak their copts
# to their (included header) dependencies and fail to build outside QIAN
QIAN_TEST_COPTS = QIAN_DEFAULT_COPTS + select({
    "//conditions:default": QIAN_GCC_TEST_FLAGS,
})

QIAN_DEFAULT_LINKOPTS = select({
    "//conditions:default": [],
})

# QIAN_RANDOM_RANDEN_COPTS blaze copts flags which are required by each
# environment to build an accelerated RandenHwAes library.
QIAN_RANDOM_RANDEN_COPTS = select({
    # APPLE
    ":cpu_darwin_x86_64": QIAN_RANDOM_HWAES_X64_FLAGS,
    ":cpu_darwin": QIAN_RANDOM_HWAES_X64_FLAGS,
    ":cpu_x64_windows_msvc": QIAN_RANDOM_HWAES_MSVC_X64_FLAGS,
    ":cpu_x64_windows": QIAN_RANDOM_HWAES_MSVC_X64_FLAGS,
    ":cpu_haswell": QIAN_RANDOM_HWAES_X64_FLAGS,
    ":cpu_ppc": ["-mcrypto"],

    # Supported by default or unsupported.
    "//conditions:default": [],
})

# QIAN_random_randen_copts_init:
#  Initialize the config targets based on cpu, os, etc. used to select
#  the required values for QIAN_RANDOM_RANDEN_COPTS
def QIAN_random_randen_copts_init():
    """Initialize the config_settings used by QIAN_RANDOM_RANDEN_COPTS."""

    # CPU configs.
    # These configs have consistent flags to enable HWAES intsructions.
    cpu_configs = [
        "ppc",
        "haswell",
        "darwin_x86_64",
        "darwin",
        "x64_windows_msvc",
        "x64_windows",
    ]
    for cpu in cpu_configs:
        native.config_setting(
            name = "cpu_%s" % cpu,
            values = {"cpu": cpu},
        )
