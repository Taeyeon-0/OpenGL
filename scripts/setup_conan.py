#!/usr/bin/env python3

"""Install this project's Conan dependencies.

Examples:
    python3 scripts/setup_conan.py --build-type Debug
    python3 scripts/setup_conan.py --build-type Release
    python3 scripts/setup_conan.py --build-type Release --no-system-deps
    python3 scripts/setup_conan.py \
        --build-type Debug \
        --profile conanprofiles/linux-gcc

On Linux, Conan installs missing system packages through ``sudo`` by default.
Use ``--no-system-deps`` when system packages should be installed manually.
"""

from __future__ import annotations

import argparse
import platform
import shutil
import subprocess
import sys
from pathlib import Path
from typing import NoReturn


BUILD_TYPES = ("Debug", "Release", "RelWithDebInfo")
PROJECT_ROOT = Path(__file__).resolve().parent.parent


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Install project dependencies with Conan."
    )
    parser.add_argument(
        "--build-type",
        choices=BUILD_TYPES,
        default="Release",
        help="Build type (default: Release).",
    )
    parser.add_argument(
        "--profile",
        dest="host_profile",
        type=Path,
        help="Conan host profile; auto-detected when omitted.",
    )
    parser.add_argument(
        "--build-profile",
        default="default",
        help="Conan build profile (default: default).",
    )
    parser.add_argument(
        "--no-system-deps",
        action="store_true",
        help="Do not ask Conan to install Linux system packages.",
    )
    return parser.parse_args()


def fail(message: str) -> NoReturn:
    print(f"setup_conan: {message}", file=sys.stderr)
    raise SystemExit(1)


def detect_host_profile() -> Path:
    system = platform.system()
    if system == "Linux":
        return PROJECT_ROOT / "conanprofiles" / "linux-gcc"
    if system == "Windows":
        return PROJECT_ROOT / "conanprofiles" / "windows-msvc"
    fail("cannot auto-detect a Conan profile; pass --profile explicitly")


def absolute_project_path(path: Path) -> Path:
    if path.is_absolute():
        return path
    return PROJECT_ROOT / path


def main() -> int:
    args = parse_args()

    conan = shutil.which("conan")
    if conan is None:
        fail("Conan was not found in PATH")

    conanfile = PROJECT_ROOT / "conanfile.py"
    if not conanfile.is_file():
        fail(f"conanfile.py was not found in {PROJECT_ROOT}")

    host_profile = absolute_project_path(args.host_profile) if args.host_profile else detect_host_profile()
    if not host_profile.is_file():
        fail(f"host profile was not found: {host_profile}")

    if args.build_profile != "default":
        build_profile = absolute_project_path(Path(args.build_profile))
        if not build_profile.is_file():
            fail(f"build profile was not found: {build_profile}")
        build_profile_arg = str(build_profile)
    else:
        build_profile_arg = args.build_profile

    conan_args = [
        conan,
        "install",
        str(PROJECT_ROOT),
        f"-pr:h={host_profile}",
        f"-pr:b={build_profile_arg}",
        f"-s:h=build_type={args.build_type}",
        "--build=missing",
    ]

    if platform.system() == "Linux" and not args.no_system_deps:
        if shutil.which("sudo") is None:
            fail("sudo was not found; install system packages manually or use --no-system-deps")
        conan_args.extend(
            [
                "-c",
                "tools.system.package_manager:mode=install",
                "-c",
                "tools.system.package_manager:sudo=True",
            ]
        )

    print(f"Installing Conan dependencies for {args.build_type}")
    try:
        subprocess.run(conan_args, cwd=PROJECT_ROOT, check=True)
    except subprocess.CalledProcessError as error:
        return error.returncode

    print("Conan setup complete")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
