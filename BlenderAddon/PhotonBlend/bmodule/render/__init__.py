import bpy

import subprocess
from pathlib import Path


class RenderProcess:
    def __init__(self):
        addon_name = __name__.split(".")[0]
        b_context = bpy.context
        b_preferences = b_context.preferences.addons[addon_name].preferences

        self.installation_path = Path(b_preferences.installation_path)
        self.scene_file_path = ""
        self.num_threads = b_context.scene.render.threads
        self.process = None

    def run(self):
        if self.process is not None:
            print("warning: process is already running")
            return

        arguments = self._generate_arguments()

        self.process = subprocess.Popen(arguments, cwd=self.installation_path.resolve())
        #self.process.

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

    def _generate_arguments(self):
        # TODO
        # executable_path = Path(b_preferences.installation_path) / "bin" / "Photon"
        return ""
