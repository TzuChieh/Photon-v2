from infra import image

from abc import ABC, abstractmethod
from pathlib import Path
from typing import Callable, List, Union, Dict


class RendererConfig:
    """
    Configuration for the rendering process.
    """
    def __init__(
            self, 
            num_threads: int = 1, 
            output_format: str = "pfm", 
            is_raw: bool = True,
            extra_args: Dict[str, str] = None):
        """
        @param num_threads Number of threads to use for rendering.
        @param output_format The image format to output (e.g., "pfm", "exr").
        @param is_raw Whether to request raw output from the renderer (no tonemapping).
        @param extra_args Additional command-line arguments to pass to the renderer (key-value pairs).
        """
        self.num_threads = num_threads
        self.output_format = output_format
        self.is_raw = is_raw
        self.extra_args = extra_args or {}


class VerificationResult:
    """
    The result of a single verification step.
    """
    def __init__(self, passed: bool, message: str = "", metrics: dict = None):
        """
        @param passed Whether the verification passed.
        @param message An optional message explaining the result (especially on failure).
        @param metrics A dictionary of numerical metrics computed during verification (e.g., MSE).
        """
        self.passed = passed
        self.message = message
        self.metrics = metrics or {}


class Verifier(ABC):
    """
    Interface for verifying the correctness of a rendered image.
    """
    @abstractmethod
    def verify(self, output_img: image.Image, output_dir: Path, case: 'RenderCase') -> VerificationResult:
        """
        Perform verification on the provided image.
        @param output_img The image to verify.
        @param output_dir The directory where debug artifacts (like error plots) should be saved.
        @param case The test case being verified.
        @return A VerificationResult object.
        """
        pass


class RenderCase:
    """
    A single test case representing a scene to be rendered and verified.
    """
    def __init__(
            self, 
            name: str, 
            scene_path: Path, 
            renderer_config: RendererConfig, 
            verifiers: Union[Verifier, List[Verifier]],
            output_filename: str = None,
            output_title: Union[str, Callable[['RenderCase', dict], str]] = None,
            case_msg: str = ""):
        """
        @param name A unique name for this test case.
        @param scene_path Path to the scene file (.p2).
        @param renderer_config Configuration for the renderer.
        @param verifiers One or more Verifier objects to check the result.
        @param output_filename Optional custom name for the output image file (without extension).
               If not provided, the case name will be used (lowercase, underscores instead of spaces).
        @param output_title Optional title for the rendered output plot. This can be either a fixed
               string or a callable with signature (case, metrics) -> str. The callable form is useful
               when the title should include verifier metrics, such as MSE or relative average error.
        @param case_msg Optional message shown in the report for this case.
        """
        self.name = name
        self.scene_path = scene_path
        self.renderer_config = renderer_config
        self.verifiers = verifiers if isinstance(verifiers, list) else [verifiers]
        self.module_name = ""
        self._test_name = ""
        self._output_dir = None
        
        if output_filename:
            self.output_filename = output_filename
        else:
            self.output_filename = name.lower().replace(" ", "_")

        self.ref_filename = None
        self.debug_output_filename = None
        self.output_title = output_title
        self.case_msg = case_msg
        self.debug_msg = ""

    def set_module_name(self, module_name: str):
        self.module_name = module_name
        self._test_name = module_name.removeprefix("tests.").replace("_", " ").title()
        from infra import paths
        self._output_dir = paths.test_output() / module_name

    def get_name(self):
        return self.name

    def get_output_dir(self):
        return self._output_dir

    def get_scene_path(self):
        return self.scene_path

    def get_output_path(self):
        return self.get_output_dir() / self.output_filename

    def get_debug_output_path(self):
        return self.get_output_dir() / self.debug_output_filename

    def get_ref_path(self):
        return self.get_output_dir() / self.ref_filename

    def get_output_title(self, metrics: dict):
        """
        @param metrics Verification metrics collected for this case.
        @return The title to use when saving the rendered output plot.
        """
        if callable(self.output_title):
            return self.output_title(self, metrics)
        if self.output_title:
            return self.output_title
        return f"{self.name} Output"

    def to_json_dict(self):
        """
        Get a json serializable dictionary for report generation.
        """
        return {
            '_test_name': self._test_name,
            '_case_name': self.name,
            '_output_dir': str(self._output_dir),
            '_scene_path': str(self.scene_path),
            'case_msg': self.case_msg,
            'output': self.output_filename,
            'debug_output': self.debug_output_filename or "",
            'ref': self.ref_filename or "",
            'debug_msg': self.debug_msg
        }


class RenderTestSuite:
    """
    A collection of RenderCase objects, typically representing a logical group of tests.
    """
    def __init__(self, name: str, res_dir: Path):
        """
        @param name Name of the suite.
        @param res_dir The directory containing resources (scenes, references) for this suite.
        """
        self.name = name
        self.res_dir = res_dir
        self.cases = []

    def add_case(self, case: RenderCase):
        """
        Add a case to the suite.
        """
        case.set_module_name(self.name)
        self.cases.append(case)

    def get_cases(self) -> List[RenderCase]:
        """
        @return All cases in this suite.
        """
        return self.cases
