from infra import image

from abc import ABC, abstractmethod
from dataclasses import dataclass
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
    def save_report_artifacts(self, output_img: image.Image, output_dir: Path, case: 'RenderCase'):
        """
        Save optional report artifacts before verification.
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


@dataclass
class VerifierReport:
    verifier: str
    passed: bool
    message: str
    metrics: dict
    plot_output_image: str
    plot_debug_image: str
    plot_ref_image: str
    debug_msg: str
    raw_output_image: str
    raw_ref_image: str

    def to_json_dict(self):
        return {
            "verifier": self.verifier,
            "passed": self.passed,
            "message": self.message,
            "metrics": self.metrics,
            "plot_output_image": self.plot_output_image,
            "plot_debug_image": self.plot_debug_image,
            "plot_ref_image": self.plot_ref_image,
            "debug_msg": self.debug_msg,
            "raw_output_image": self.raw_output_image,
            "raw_ref_image": self.raw_ref_image
            }


def get_metric(verifier_reports: List[VerifierReport], metric_name: str):
    for report in verifier_reports:
        if metric_name in report.metrics:
            return report.metrics[metric_name]
    raise KeyError(f"cannot find verifier metric <{metric_name}>")


class RenderCase:
    """
    A single render test case. Also stores transient report state while the case is running.
    """
    def __init__(
            self, 
            name: str, 
            scene_path: Path, 
            renderer_config: RendererConfig, 
            verifiers: Union[Verifier, List[Verifier]],
            output_filename: str = None,
            output_title: Union[str, Callable[['RenderCase', List[VerifierReport]], str]] = None,
            case_msg: str = ""):
        """
        @param name A unique name for this test case.
        @param scene_path Path to the scene file (.p2).
        @param renderer_config Configuration for the renderer.
        @param verifiers One or more Verifier objects to check the result.
        @param output_filename Optional custom name for the output image file (without extension).
               If not provided, the case name will be used (lowercase, underscores instead of spaces).
        @param output_title Optional title for the rendered output plot. This can be either a fixed
               string or a callable with signature (case, verifier_reports) -> str. The callable form
               is useful when the title should include verifier metrics.
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

        self.plot_ref_image_filename = None
        self.plot_debug_image_filename = None
        self.raw_output_image_filename = ""
        self.raw_ref_image_filename = ""
        self.output_title = output_title
        self.case_msg = case_msg
        self.debug_msg = ""
        self.verifier_reports: List[VerifierReport] = []
        self._active_verifier_name = None
        self._active_verifier_file_stem = None

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
        if not self.plot_debug_image_filename:
            raise ValueError("debug output filename is not set")
        return self.get_output_dir() / self.plot_debug_image_filename

    def get_ref_path(self):
        if not self.plot_ref_image_filename:
            raise ValueError("reference filename is not set")
        return self.get_output_dir() / self.plot_ref_image_filename

    def get_output_title(self):
        """
        @return The title to use when saving the rendered output plot.
        """
        if callable(self.output_title):
            return self.output_title(self, self.verifier_reports)
        if self.output_title:
            return self.output_title
        return f"{self.name} Output"

    def reset_report_state(self):
        self.plot_ref_image_filename = None
        self.plot_debug_image_filename = None
        self.debug_msg = ""
        self.raw_output_image_filename = ""
        self.raw_ref_image_filename = ""
        self.verifier_reports = []
        self._active_verifier_name = None
        self._active_verifier_file_stem = None

    def begin_verifier_report(self, verifier_name: str, verifier_index: int):
        self.plot_ref_image_filename = None
        self.plot_debug_image_filename = None
        self.debug_msg = ""
        self.raw_output_image_filename = self.output_filename
        self.raw_ref_image_filename = ""
        self._active_verifier_name = f"{verifier_name} {verifier_index + 1}"
        self._active_verifier_file_stem = f"{self.output_filename}_{verifier_index + 1:02d}_{verifier_name.lower()}"

    def get_active_verifier_file_stem(self):
        if not self._active_verifier_file_stem:
            raise ValueError("active verifier file stem is not set")
        return self._active_verifier_file_stem

    def set_plot_ref_image_filename(self, filename: str):
        self.plot_ref_image_filename = filename

    def clear_plot_ref_image_filename(self):
        self.plot_ref_image_filename = None

    def set_plot_debug_image_filename(self, filename: str):
        self.plot_debug_image_filename = filename

    def set_debug_msg(self, message: str):
        self.debug_msg = message

    def set_raw_output_image_filename(self, filename: str):
        self.raw_output_image_filename = filename

    def set_raw_ref_image_filename(self, filename: str):
        self.raw_ref_image_filename = filename

    def end_verifier_report(self, result: VerificationResult):
        if not self._active_verifier_name:
            raise ValueError("active verifier name is not set")

        self.verifier_reports.append(VerifierReport(
            verifier=self._active_verifier_name,
            passed=result.passed,
            message=result.message,
            metrics=result.metrics,
            plot_output_image=self.output_filename,
            plot_debug_image=self.plot_debug_image_filename or "",
            plot_ref_image=self.plot_ref_image_filename or "",
            debug_msg=self.debug_msg,
            raw_output_image=self.raw_output_image_filename or "",
            raw_ref_image=self.raw_ref_image_filename or ""
            ))

        self._active_verifier_name = None
        self._active_verifier_file_stem = None

    def sync_primary_report_fields(self):
        """
        Sync case-level summary fields from the collected per-verifier reports.
        """
        self.plot_ref_image_filename = None
        self.plot_debug_image_filename = None
        self.debug_msg = ""
        self.raw_output_image_filename = self.output_filename
        self.raw_ref_image_filename = ""

        for verifier_report in reversed(self.verifier_reports):
            if verifier_report.plot_ref_image and not self.plot_ref_image_filename:
                self.plot_ref_image_filename = verifier_report.plot_ref_image
            if verifier_report.plot_debug_image and not self.plot_debug_image_filename:
                self.plot_debug_image_filename = verifier_report.plot_debug_image
            if verifier_report.debug_msg and not self.debug_msg:
                self.debug_msg = verifier_report.debug_msg

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
            'plot_output_image': self.output_filename,
            'plot_debug_image': self.plot_debug_image_filename or "",
            'plot_ref_image': self.plot_ref_image_filename or "",
            'debug_msg': self.debug_msg,
            'raw_output_image': self.raw_output_image_filename or "",
            'raw_ref_image': self.raw_ref_image_filename or "",
            'verifier_reports': [report.to_json_dict() for report in self.verifier_reports]
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
