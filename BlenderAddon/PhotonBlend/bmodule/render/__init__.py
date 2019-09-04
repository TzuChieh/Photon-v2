import bpy

import subprocess
from pathlib import Path


class RenderProcess:
    def __init__(self):
        addon_name = __name__.split(".")[0]
        b_context = bpy.context
        b_preferences = b_context.preferences.addons[addon_name].preferences

        self.installation_path = str(Path(b_preferences.installation_path).resolve())
        self.process = None
        self.arguments = {}

        self.set_num_render_threads(b_context.scene.render.threads)

    def __del__(self):
        self.exit()

    def run(self):
        if self.process is not None:
            print("warning: process is already running")
            return

        argument_string = self._generate_argument_string()

        print(argument_string)
        print(self.installation_path)

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
        self._set_argument("-s", scene_file_path)

    def set_image_output_path(self, image_output_path):
        self._set_argument("-o", image_output_path)

    def set_image_format(self, image_format):
        self._set_argument("-of", image_format)

    def set_num_render_threads(self, num_render_threads):
        self._set_argument("-t", str(num_render_threads))

    def request_intermediate_output(self, **options):
        values = ""
        values += options.get('interval', str(2))
        values += options.get('unit', 's')
        values += " "
        values += options.get('is_overwriting', True)

        self._set_argument("-p", values)

    def request_raw_output(self):
        self._set_argument("--raw", "")

    def _generate_argument_string(self):
        argument_string = ""

        executable_path = (Path(self.installation_path) / "bin" / "PhotonCLI").resolve()
        argument_string += str(executable_path) + " "

        for key, value in self.arguments.items():
            argument_string += key + " " + value + " "

        return argument_string

    def _set_argument(self, key, value):
        self.arguments[key] = value
