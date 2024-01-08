import subprocess
import sys
from pathlib import Path


def engine_build():
    return Path.cwd()

def engine_resources():
    return engine_build() / "Photon-v2-Resource" / "Resource"

def test_resources():
    return engine_build() / "Photon-v2-Resource" / "RenderTest"

def test_output():
    return engine_build() / "RenderTest" / "test_output"

def report_output():
    return engine_build() / "RenderTest" / "report_output"

def renderer_executable():
    executable_filename = "PhotonCLI.exe"
    if sys.platform != "win32":
        executable_filename = "PhotonCLI"
    return engine_build() / "bin" / executable_filename
