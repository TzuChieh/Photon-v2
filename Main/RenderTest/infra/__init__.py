from infra import paths

import sys
import json
import copy


class TestCase:
    def __init__(self, module_name, case_name, scene_path):
        super().__init__()

        if module_name not in sys.modules:
            raise ValueError("specified name \"%s\" is not a module" % module_name)

        self._test_name = module_name.removeprefix("tests.").replace("_", " ").title()
        self._case_name = case_name
        self._output_dir = paths.test_output() / module_name
        self._scene_path = scene_path

        self.case_msg = ""
        self.output = ""
        self.debug_output = ""
        self.debug_msg = ""
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

    def to_json_dict(self):
        """
        Get a json serializable dictionary containing information from this object.
        """
        out_case = copy.copy(self)
        
        # `pathlib.Path` is not serializable to json by default, convert to string manually
        out_case._output_dir = str(self._output_dir)
        out_case._scene_path = str(self._scene_path)

        return out_case.__dict__

