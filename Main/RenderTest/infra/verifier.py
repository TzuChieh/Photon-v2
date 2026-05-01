from infra import image
from infra.core import Verifier, VerificationResult, RenderCase
from infra.resource_cache import ResourceCache

import numpy as np
from pytest import approx
from pathlib import Path
from typing import Union


class MSEVerifier(Verifier):
    """
    Checks the Mean Squared Error (MSE) against a reference image or constant.
    """
    def __init__(self, ref: Union[Path, float], threshold: float):
        self.ref = ref
        self.threshold = threshold

    def verify(self, output_img: image.Image, output_dir: Path, case: RenderCase) -> VerificationResult:
        if isinstance(self.ref, Path):
            ref_img = ResourceCache.get_image(self.ref)
            mse = image.mse_of(output_img, ref_img)
        else:
            mse = ((output_img.values - self.ref)**2).mean()

        passed = mse < self.threshold
        
        return VerificationResult(
            passed=passed,
            message=f"MSE={mse:.6f} (max={self.threshold:.6f})" if not passed else "",
            metrics={"mse": mse}
        )


class RelAvgVerifier(Verifier):
    """
    Checks the Relative error of the Averaged value (RelAvg) against a reference or constant.
    """
    def __init__(self, ref: Union[Path, float], threshold: float):
        self.ref = ref
        self.threshold = threshold

    def verify(self, output_img: image.Image, output_dir: Path, case: RenderCase) -> VerificationResult:
        if isinstance(self.ref, Path):
            ref_img = ResourceCache.get_image(self.ref)
            rel_avg = image.re_avg_of(output_img, ref_img)
        else:
            avg_actual = np.average(output_img.values)
            rel_avg = (avg_actual - self.ref) / self.ref

        passed = abs(rel_avg) < self.threshold
        
        return VerificationResult(
            passed=passed,
            message=f"RelAvg={rel_avg*100:.3f}% (max={self.threshold*100:.3f}%)" if not passed else "",
            metrics={"rel_avg": rel_avg}
        )


class PerPixelVerifier(Verifier):
    """
    Performs a strict per-pixel comparison against a constant value.
    """
    def __init__(self, expected_value: float, tolerance: float = 1e-8):
        self.expected_value = expected_value
        self.tolerance = tolerance

    def verify(self, output_img: image.Image, output_dir: Path, case: RenderCase) -> VerificationResult:
        passed = True
        for value in np.nditer(output_img.values):
            if value != approx(self.expected_value, abs=self.tolerance):
                passed = False
                break
        
        return VerificationResult(
            passed=passed,
            message=f"Per-pixel check failed (expected {self.expected_value}, tol={self.tolerance})" if not passed else "",
            metrics={"expected": self.expected_value}
        )

class VisualErrorVerifier(Verifier):
    """
    Generates scaled error plots for visualization.
    Reference plots are handled by the infrastructure (conftest.py).
    """
    def __init__(
            self, 
            ref: Union[Path, float], 
            error_scale: float = 100.0,
            error_output_filename: str = None,
            ref_output_filename: str = "ref",
            error_title=None,
            ref_title: str = "Reference Image"):
        """
        @param ref Reference image path or scalar value to compare against.
                   If this is a path, a reference plot is generated for the report.
        @param error_output_filename Custom name for error plot. Defaults to {case.output}_error.
        @param ref_output_filename Name used for reference plot in the report. Defaults to "ref".
        @param error_title Optional title for the error plot. May be a callable taking the case.
        @param ref_title Title for the reference plot.
        """
        self.ref = ref
        self.error_scale = error_scale
        self.error_output_filename = error_output_filename
        self.ref_output_filename = ref_output_filename
        self.error_title = error_title
        self.ref_title = ref_title

    def has_image_ref(self):
        return isinstance(self.ref, Path)

    def has_scalar_ref(self):
        return not self.has_image_ref()

    def get_image_ref_path(self):
        if not self.has_image_ref():
            raise ValueError("VisualErrorVerifier does not have an image reference")
        return self.ref

    def get_ref_output_filename(self):
        return self.ref_output_filename

    def get_ref_title(self):
        return self.ref_title

    def verify(self, output_img: image.Image, output_dir: Path, case: RenderCase) -> VerificationResult:
        error_img = image.Image(output_img.get_width(), output_img.get_height(), output_img.num_components())

        # Compatibility metadata for reporter
        case.ref_filename = self.ref_output_filename if self.has_image_ref() else None
        case.debug_output_filename = self.error_output_filename or f"{case.output_filename}_error"

        if self.has_image_ref():
            ref_img = ResourceCache.get_image(self.ref)
            error_img.values = (output_img.values - ref_img.values)
        else:
            error_img.values = (output_img.values - self.ref)


        error_img.values = np.abs(error_img.values) * self.error_scale
        error_img = error_img.to_summed_absolute_components()
        
        debug_output_path = output_dir / case.debug_output_filename
        if callable(self.error_title):
            error_title = self.error_title(case)
        else:
            error_title = self.error_title or f"{case.name} {self.error_scale:g}X Absolute Error"
        error_img.save_pseudocolor_plot(debug_output_path, error_title)

        return VerificationResult(passed=True)
