import subprocess
import sys
from pathlib import Path


def engine_resources():
    return Path(".") / "Photon-v2-Resource" / "Resource"

def test_resources():
    return Path(".") / "Photon-v2-Resource" / "RenderTest"

def test_output():
    return Path(".") / "RenderTest" / "test_output"

def renderer_executable():
    executable_filename = "PhotonCLI.exe"
    if sys.platform != "win32":
        executable_filename = "PhotonCLI"
    return Path(".") / "bin" / executable_filename
