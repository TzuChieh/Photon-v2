from infra import paths

import subprocess
from pathlib import Path


def open_default_render_process(scene_path, output_path, num_threads=1):
    process = RenderProcess()
    process.set_scene_file_path(scene_path)
    process.set_image_output_path(output_path)
    process.set_num_render_threads(num_threads)
    process.set_image_format("pfm")
    process.request_raw_output()

    # Output path may contain new (non-existing) folders, create them first so ordinary file save
    # operations will not fail
    Path(output_path).parents[0].mkdir(parents=True, exist_ok=True)

    return process


class RenderProcess:
    """
    Create and cleanup a render process for a test. Basically calls `PhotonCLI.exe` for rendering. Contains various
    utility methods for controlling the renderer.
    """

    def __init__(self):
        self.process = None
        self.arguments = {}

    def __del__(self):
        self.exit()

    def run_and_wait(self):
        assert self.process is None

        argument_string = self._generate_argument_string()

        self.process = subprocess.Popen(argument_string)
        self.process.wait()

    def exit(self):
        if self.process is None:
            return

        self.process.terminate()

        timeout_seconds = 10
        try:
            self.process.wait(timeout=timeout_seconds)
        except subprocess.TimeoutExpired as e:
            print("warning: process does not terminate, killing")
            self.process.kill()

    def set_scene_file_path(self, scene_file_path):
        self._set_argument("-s", "\"" + str(scene_file_path) + "\"")

    def set_image_output_path(self, image_output_path):
        self._set_argument("-o", "\"" + str(image_output_path) + "\"")

    def set_image_format(self, image_format):
        self._set_argument("-of", image_format)

    def set_num_render_threads(self, num):
        self._set_argument("-t", str(num))

    def request_raw_output(self):
        self._set_argument("--raw", "")

    def is_running(self):
        if self.process is None:
            return False

        return self.process.poll() is None

    def _generate_argument_string(self):
        argument_string = ""

        executable_path = paths.renderer_executable().resolve(strict=True)
        argument_string += str(executable_path) + " "

        for key, value in self.arguments.items():
            argument_string += key + " " + value + " "

        return argument_string

    def _set_argument(self, key, value):
        self.arguments[key] = value

