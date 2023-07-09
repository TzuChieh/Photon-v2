import bpy

import subprocess
from pathlib import Path


class RenderProcess:
    """
    Create and cleanup a render process. Basically calls `PhotonCLI.exe` for rendering. Contains various
    utility methods for controlling the renderer.
    """

    def __init__(self):
        addon_name = "PhotonBlend"
        b_context = bpy.context
        b_preferences = b_context.preferences.addons[addon_name].preferences

        if b_preferences.installation_path:
            self.installation_path = str(Path(b_preferences.installation_path).resolve())
        else:
            self.installation_path = None
            print("*** Please set the installation path for Photon renderer in addon preferences ***")
        
        self.process = None
        self.arguments = {}

    def __del__(self):
        self.exit()

    def run(self):
        if self.process is not None:
            print("warning: process is already running")
            return

        if not self.installation_path:
            print("warning: cannot run render process, no installation path is set")
            return

        argument_string = self._generate_argument_string()

        print("Using renderer installation: %s" % self.installation_path)
        print("Renderer arguments: %s" % argument_string)

        self.process = subprocess.Popen(argument_string, cwd=self.installation_path)

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

    def request_intermediate_output(self, **options):
        values = ""

        values += str(options.get('interval', 2))
        values += options.get('unit', 's')
        values += " "

        is_overwriting = options.get('is_overwriting', True)
        values += "true" if is_overwriting else "false"

        self._set_argument("-p", values)

    def set_port(self, port):
        self._set_argument("--port", str(port))

    def request_raw_output(self):
        self._set_argument("--raw", "")

    def is_running(self):
        if self.process is None:
            return False

        return self.process.poll() is None

    def _generate_argument_string(self):
        argument_string = ""

        executable_path = (Path(self.installation_path) / "bin" / "PhotonCLI").resolve()
        argument_string += str(executable_path) + " "

        for key, value in self.arguments.items():
            argument_string += key + " " + value + " "

        return argument_string

    def _set_argument(self, key, value):
        self.arguments[key] = value
