from infra import paths

import sys
import json


class TestCase:
    def __init__(self, module_name, case_name, scene_path):
        super().__init__()

        if module_name not in sys.modules:
            raise ValueError("specified name \"%s\" is not a module" % module_name)

        self._case_name = case_name
        self._output_dir = paths.test_output() / module_name
        self._scene_path = scene_path

        self.output = ""
        self.debug_output = ""
        self.ref = ""

    def get_name(self):
        return self._case_name
    
    def get_output_dir(self):
        return self._output_dir
    
    def get_scene_path(self):
        return self._scene_path
    
    def get_output_path(self):
        return self.get_output_dir() / self.output
    
    def get_debug_output_path(self):
        return self.get_output_dir()  / self.debug_output
    
    def get_ref_path(self):
        return self.get_output_dir()  / self.ref

    def to_json(self):
        return json.dumps(self.__dict__)
