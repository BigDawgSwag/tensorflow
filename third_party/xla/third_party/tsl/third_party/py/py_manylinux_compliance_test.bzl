""" Macros for manylinux compliance verification test. """

load("@rules_python//python:py_test.bzl", "py_test")

def verify_manylinux_compliance_test(
        name,
        wheel,
        aarch64_compliance_tag,
        x86_64_compliance_tag,
        test_tags = []):
    verify_manylinux_compliance_target = Label("//third_party/py:verify_manylinux_compliance")
    py_test(
        name = name,
        srcs = [Label("//third_party/py:manylinux_compliance_test")],
        data = [
            wheel,
        ],
        deps = [
            verify_manylinux_compliance_target,
        ],
        args = [
            "--wheel-path=$(location {})".format(wheel),
            "--aarch64-compliance-tag={}".format(aarch64_compliance_tag),
            "--x86_64-compliance-tag={}".format(x86_64_compliance_tag),
        ],
        main = "manylinux_compliance_test.py",
        tags = ["manual"] + test_tags,
    )
