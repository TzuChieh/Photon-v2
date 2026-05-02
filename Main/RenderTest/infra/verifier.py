from infra import image
from infra.core import Verifier, VerificationResult, RenderCase
from infra.resource_cache import ResourceCache

import numpy as np
from pytest import approx
from pathlib import Path
from typing import Callable, Union


RefImage = Union[Path, image.Image, float]


class RefVerifier(Verifier):
    def __init__(self, ref: RefImage):
        self.ref_image_path = None
        self.ref_image = None
        self.ref_scalar = None

        if isinstance(ref, Path):
            self.ref_image_path = ref
        elif isinstance(ref, image.Image):
            self.ref_image = ref
        else:
            self.ref_scalar = ref

    def has_image_ref(self):
        return self.ref_image_path is not None or self.ref_image is not None

    def has_scalar_ref(self):
        return self.ref_scalar is not None

    def get_image_ref_path(self):
        if not self._has_path_ref():
            raise ValueError("verifier does not have a path reference")
        return self.ref_image_path

    def get_image_ref(self):
        if not self.has_image_ref():
            raise ValueError("verifier does not have an image reference")
        if self._has_path_ref():
            return ResourceCache.get_image(self.ref_image_path)
        return self.ref_image

    def get_ref_scalar(self):
        if not self.has_scalar_ref():
            raise ValueError("verifier does not have a scalar reference")
        return self.ref_scalar
    
    def _has_path_ref(self):
        return self.ref_image_path is not None

    def get_ref_image_for_output(self, output_img: image.Image):
        if self.has_image_ref():
            return self.get_image_ref()

        ref_img = image.Image(output_img.get_width(), output_img.get_height(), output_img.num_components())
        ref_img.fill(self.get_ref_scalar())
        return ref_img

    def save_compare_ref_raw(self, output_img: image.Image, output_dir: Path, case: RenderCase):
        ref_img = self.get_ref_image_for_output(output_img)
        ref_raw_filename = f"{case.output_filename}_{type(self).__name__.lower()}_ref_raw"
        ref_img.save_pfm(output_dir / ref_raw_filename)
        case.set_raw_output_image_filename(case.output_filename)
        case.set_raw_ref_image_filename(ref_raw_filename)


class MSEVerifier(RefVerifier):
    """
    Checks the Mean Squared Error (MSE) against a reference image or constant.
    """
    def __init__(self, ref: RefImage, threshold: float):
        super().__init__(ref)
        self.threshold = threshold

    def verify(self, output_img: image.Image, output_dir: Path, case: RenderCase) -> VerificationResult:
        if self.has_image_ref():
            ref_img = self.get_image_ref()
            mse = image.mse_of(output_img, ref_img)
        else:
            mse = ((output_img.values - self.get_ref_scalar())**2).mean()

        passed = mse < self.threshold
        
        return VerificationResult(
            passed=passed,
            message=f"MSE={mse:.6f} (max={self.threshold:.6f})" if not passed else "",
            metrics={"mse": mse})


class RelMeanVerifier(RefVerifier):
    """
    Checks the Relative error of the mean value against a reference or constant.
    """
    def __init__(self, ref: RefImage, threshold: float):
        super().__init__(ref)
        self.threshold = threshold

    def verify(self, output_img: image.Image, output_dir: Path, case: RenderCase) -> VerificationResult:
        if self.has_image_ref():
            ref_img = self.get_image_ref()
            rel_mean = image.re_avg_of(output_img, ref_img)
        else:
            avg_actual = np.average(output_img.values)
            ref_scalar = self.get_ref_scalar()
            rel_mean = (avg_actual - ref_scalar) / ref_scalar

        passed = abs(rel_mean) < self.threshold
        
        return VerificationResult(
            passed=passed,
            message=f"RelMean={rel_mean*100:.3f}% (max={self.threshold*100:.3f}%)" if not passed else "",
            metrics={"rel_mean": rel_mean})


class PerPixelVerifier(RefVerifier):
    """
    Performs a strict per-pixel comparison against a constant value.
    """
    def __init__(self, expected_value: float, tolerance: float = 1e-8):
        super().__init__(expected_value)
        self.tolerance = tolerance

    def verify(self, output_img: image.Image, output_dir: Path, case: RenderCase) -> VerificationResult:
        expected_value = self.get_ref_scalar()
        passed = True
        for value in np.nditer(output_img.values):
            if value != approx(expected_value, abs=self.tolerance):
                passed = False
                break
        
        return VerificationResult(
            passed=passed,
            message=f"Per-pixel check failed (expected {expected_value}, tol={self.tolerance})" if not passed else "",
            metrics={"expected": expected_value})


class MeanDiffVerifier(RefVerifier):
    """
    Checks image mean against an expected value and stores useful extrema in case debug info.
    """
    def __init__(self, expected_value: float, threshold: float):
        super().__init__(expected_value)
        self.threshold = threshold

    def verify(self, output_img: image.Image, output_dir: Path, case: RenderCase) -> VerificationResult:
        mean_diff = np.mean(output_img.values) - self.get_ref_scalar()
        case.set_debug_msg("mean diff = %.8f, max pixel = %.8f, min pixel = %.8f" % (
            mean_diff, np.max(output_img.values), np.min(output_img.values)))

        passed = abs(mean_diff) < self.threshold
        return VerificationResult(
            passed=passed,
            message=f"MeanDiff={mean_diff:.8f} (max={self.threshold:.8f})" if not passed else "",
            metrics={"mean_diff": mean_diff})


class VisualErrorVerifier(RefVerifier):
    """
    Generates scaled error plots for visualization.
    Reference plots are handled by the infrastructure (conftest.py).
    """
    def __init__(
            self, 
            ref: RefImage,
            error_scale: float = 100.0,
            error_output_filename: str = None,
            ref_output_filename: str = "ref",
            error_title=None,
            ref_title: str = "Reference Image",
            color_max: float = 100.0):
        """
        @param ref Reference image path, image object, or scalar value to compare against.
                   If this is a path or image object, a reference plot is generated for the report.
        @param error_output_filename Custom name for error plot. Defaults to {case.output}_error.
        @param ref_output_filename Name used for reference plot in the report. Defaults to "ref".
        @param error_title Optional title for the error plot. May be a callable taking the case.
        @param ref_title Title for the reference plot.
        @param color_max Maximum color scale value for the error plot.
        """
        super().__init__(ref)
        self.error_scale = error_scale
        self.error_output_filename = error_output_filename
        self.ref_output_filename = ref_output_filename
        self.error_title = error_title
        self.ref_title = ref_title
        self.color_max = color_max

    def save_ref_plot(self, output_dir: Path):
        if not self.has_image_ref():
            return
        self.get_image_ref().save_plot(output_dir / self.ref_output_filename, self.ref_title, create_dirs=True)

    def get_ref_output_filename(self):
        return self.ref_output_filename

    def get_ref_title(self):
        return self.ref_title

    def get_ref_key(self):
        return self.get_image_ref_path() if self._has_path_ref() else self.get_ref_output_filename()

    def verify(self, output_img: image.Image, output_dir: Path, case: RenderCase) -> VerificationResult:
        error_img = image.Image(output_img.get_width(), output_img.get_height(), output_img.num_components())

        if self.has_image_ref():
            case.set_plot_ref_image_filename(self.ref_output_filename)
        else:
            case.clear_plot_ref_image_filename()
        case.set_plot_debug_image_filename(self.error_output_filename or f"{case.output_filename}_error")

        if self.has_image_ref():
            ref_img = self.get_image_ref()
            error_img.values = (output_img.values - ref_img.values)
        else:
            error_img.values = (output_img.values - self.get_ref_scalar())

        error_img.values = np.abs(error_img.values) * self.error_scale
        error_img = error_img.to_summed_absolute_components()
        
        debug_output_path = output_dir / case.plot_debug_image_filename
        if callable(self.error_title):
            error_title = self.error_title(case)
        else:
            error_title = self.error_title or f"{case.name} {self.error_scale:g}X Absolute Error"
        error_img.save_pseudocolor_plot(debug_output_path, error_title, color_max=self.color_max)

        return VerificationResult(passed=True)


class PseudocolorPlotVerifier(Verifier):
    """
    Saves a pseudocolor debug plot for a derived single-component image.
    """
    def __init__(
            self,
            transform: Callable[[image.Image], image.Image],
            output_filename: str = None,
            title=None,
            color_min: float = 0.0,
            color_max: float = 100.0,
            color_map: str = 'nipy_spectral'):
        self.transform = transform
        self.output_filename = output_filename
        self.title = title
        self.color_min = color_min
        self.color_max = color_max
        self.color_map = color_map

    def verify(self, output_img: image.Image, output_dir: Path, case: RenderCase) -> VerificationResult:
        case.set_plot_debug_image_filename(self.output_filename or f"{case.output_filename}_error")
        debug_img = self.transform(output_img)
        title = self.title(case) if callable(self.title) else (self.title or f"{case.name} Debug Output")
        debug_img.save_pseudocolor_plot(
            output_dir / case.plot_debug_image_filename,
            title,
            color_min=self.color_min,
            color_max=self.color_max,
            color_map=self.color_map)
        return VerificationResult(passed=True)
