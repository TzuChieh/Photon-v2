import argparse
import subprocess
import sys
from pathlib import Path

from utility import config


def main():
    parser = argparse.ArgumentParser(
        description="Set up Photon, generate project files, and build in Release mode.")
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
    args = parser.parse_args()

    project_dir = Path(__file__).resolve().parent.parent
    setup_config = config.get_setup_config()
    build_dir = project_dir / setup_config["General"]["BuildDirectory"]

    if sys.platform == "win32":
        setup_command = ["cmd", "/c", str(project_dir / "setup.bat")]
    else:
        setup_command = ["bash", str(project_dir / "setup.sh")]

    if not args.no_setup:
        subprocess.run(
            setup_command,
            cwd=project_dir,
            check=True)
    if not args.no_cmake:
        subprocess.run(
            ["cmake", "--fresh", "-S", str(project_dir), "-B", str(build_dir),
             "-DCMAKE_BUILD_TYPE=Release", "-DPH_BUILD_EDITOR_JNI=OFF"],
            check=True)
    build_command = ["cmake", "--build", str(build_dir), "--config", "Release"]
    if args.target:
        build_command += ["--target", args.target]
    subprocess.run(build_command, check=True)


if __name__ == "__main__":
    main()
