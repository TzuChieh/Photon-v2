import argparse
import subprocess
import sys
from pathlib import Path

from utility import config


def extract_cmake_definitions(args):
    cmake_defs = []
    remaining_args = []
    i = 0

    while i < len(args):
        arg = args[i]
        if arg == "-D":
            i += 1
            if i == len(args):
                raise ValueError("-D requires a CMake definition.")
            cmake_defs.append(f"-D{args[i]}")
        elif arg.startswith("-D") and len(arg) > 2:
            cmake_defs.append(arg)
        else:
            remaining_args.append(arg)
        i += 1

    return cmake_defs, remaining_args


def main():
    parser = argparse.ArgumentParser(
        description="Set up Photon, generate project files, and build in Release mode.",
        epilog="CMake definitions in -DNAME=VALUE or -D NAME=VALUE form are forwarded to CMake.")
    parser.add_argument(
        "--target",
        help="Build a specific CMake target. Defaults to all targets.")
    parser.add_argument(
        "--no-setup",
        action="store_true",
        help="Skip setup step.")
    parser.add_argument(
        "--no-cmake",
        action="store_true",
        help="Skip CMake project configuration and generation steps.")
    parser.add_argument(
        "--py-ver",
        help="Python version for setup's virtual environment, e.g. 3.11.")
    args, extra_args = parser.parse_known_args()

    try:
        cmake_defs, extra_args = extract_cmake_definitions(extra_args)
    except ValueError as e:
        parser.error(str(e))

    if extra_args:
        parser.error(f"unrecognized arguments: {' '.join(extra_args)}")

    if args.no_cmake and cmake_defs:
        parser.error("CMake definitions require CMake configuration; remove --no-cmake.")

    project_dir = Path(__file__).resolve().parent.parent
    setup_config = config.get_setup_config()
    build_dir = project_dir / setup_config["General"]["BuildDirectory"]

    if sys.platform == "win32":
        setup_command = ["cmd", "/c", str(project_dir / "setup.bat")]
    else:
        setup_command = ["bash", str(project_dir / "setup.sh")]

    if args.py_ver:
        setup_command += ["--py-ver", args.py_ver]

    if not args.no_setup:
        subprocess.run(
            setup_command,
            cwd=project_dir,
            check=True)
        
    if not args.no_cmake:
        cmake_command = [
            "cmake", "--fresh", "-S", str(project_dir), "-B", str(build_dir),
            "-DCMAKE_BUILD_TYPE=Release", "-DPH_BUILD_EDITOR_JNI=OFF",
            *cmake_defs]
        subprocess.run(
            cmake_command,
            check=True)
        
    build_command = ["cmake", "--build", str(build_dir), "--config", "Release"]
    if args.target:
        build_command += ["--target", args.target]
    subprocess.run(build_command, check=True)


if __name__ == "__main__":
    main()
