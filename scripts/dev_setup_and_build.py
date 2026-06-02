import argparse
import subprocess
import sys
from pathlib import Path

from utility import config


def main():
    parser = argparse.ArgumentParser(
        description="Set up Photon, generate project files, and build in Release mode.")
    parser.parse_args()

    project_dir = Path(__file__).resolve().parent.parent
    setup_config = config.get_setup_config()
    build_dir = project_dir / setup_config["General"]["BuildDirectory"]

    if sys.platform == "win32":
        setup_command = ["cmd", "/c", str(project_dir / "setup.bat")]
    else:
        setup_command = ["bash", str(project_dir / "setup.sh")]

    subprocess.run(
        setup_command,
        cwd=project_dir,
        check=True)
    subprocess.run(
        ["cmake", "--fresh", "-S", str(project_dir), "-B", str(build_dir),
         "-DCMAKE_BUILD_TYPE=Release", "-DPH_BUILD_EDITOR_JNI=OFF"],
        check=True)
    subprocess.run(
        ["cmake", "--build", str(build_dir), "--config", "Release"],
        check=True)


if __name__ == "__main__":
    main()
