from infra import image
from infra.core import Verifier, VerificationResult, RenderCase
from infra.resource_cache import ResourceCache

import math
import numpy as np
from pytest import approx
from pathlib import Path
from typing import Callable, Union


RefInput = Union[Path, image.Image, float, int]


class RefSource:
    """
    Lazily provides a reference image or scalar value.
    """
    def __init__(self, ref: RefInput):
        """
        @param ref Reference path, in-memory image, or scalar value.
        """
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
        """
        @return True if the reference is an image path or in-memory image.
        """
        return self.ref_image_path is not None or self.ref_image is not None

    def has_scalar_ref(self):
        """
        @return True if the reference is a scalar value.
        """
        return self.ref_scalar is not None

    def get_image_ref_path(self):
        """
        @return Source path of the reference image used for comparison.
        @exception ValueError If this source is not path-backed.
        """
        if not self._has_path_ref():
            raise ValueError("verifier does not have a path reference")
        return self.ref_image_path

    def get_image_ref(self):
        """
        @return Reference image, loaded through `ResourceCache` if path-backed.
        @exception ValueError If this source is scalar-backed.
        """
        if not self.has_image_ref():
            raise ValueError("verifier does not have an image reference")
        if self._has_path_ref():
            return ResourceCache.get_image(self.ref_image_path)
        return self.ref_image

    def get_ref_scalar(self):
        """
        @return Scalar reference value.
        @exception ValueError If this source is image-backed.
        """
        if not self.has_scalar_ref():
            raise ValueError("verifier does not have a scalar reference")
        return self.ref_scalar
    
    def _has_path_ref(self):
        return self.ref_image_path is not None

    def get_ref_image_for_output(self, output_img: image.Image):
        """
        @param output_img Rendered output image used for scalar-reference dimensions.
        @return An image reference with the same dimensions/components as `output_img`.
        """
        if self.has_image_ref():
            return self.get_image_ref()

        ref_img = image.Image(output_img.get_width(), output_img.get_height(), output_img.num_components())
        ref_img.fill(self.get_ref_scalar())
        return ref_img


class RefVerifier(Verifier):
    """
    Base class for verifiers that compare against an image or scalar reference.
    """
    def __init__(self, ref: RefInput):
        """
        @param ref Reference path, in-memory image, or scalar value.
        """
        self.ref_source = RefSource(ref)

    def get_ref_source(self):
        """
        @return Primary reference source.
        """
        return self.ref_source

    def save_report_artifacts(self, output_img: image.Image, output_dir: Path, case: RenderCase):
        """
        Save the primary reference as raw PFM for report comparison.
        """
        ref_img = self.get_ref_source().get_ref_image_for_output(output_img)
        ref_raw_filename = f"{case.get_active_verifier_file_stem()}_ref_raw"
        ref_img.save_pfm(output_dir / ref_raw_filename)
        case.set_raw_output_image_filename(case.output_filename)
        case.set_raw_ref_image_filename(ref_raw_filename)


class MSEVerifier(RefVerifier):
    """
    Checks the Mean Squared Error (MSE) against a reference image or constant.
    """
    METRIC_MSE = "mse"

    def __init__(self, ref: RefInput, threshold: float):
        super().__init__(ref)
        self.threshold = threshold

    def verify(self, output_img: image.Image, output_dir: Path, case: RenderCase) -> VerificationResult:
        ref_source = self.get_ref_source()
        if ref_source.has_image_ref():
            ref_img = ref_source.get_image_ref()
            mse = image.mse_of(output_img, ref_img)
        else:
            mse = ((output_img.values - ref_source.get_ref_scalar())**2).mean()

        passed = mse < self.threshold
        
        return VerificationResult(
            passed=passed,
            message=f"MSE={mse:.6f} (max={self.threshold:.6f})" if not passed else "",
            metrics={self.METRIC_MSE: mse})


class RelMeanVerifier(RefVerifier):
    """
    Checks the Relative error of the mean value against a reference or constant.
    """
    METRIC_REL_MEAN = "rel_mean"

    def __init__(self, ref: RefInput, threshold: float):
        super().__init__(ref)
        self.threshold = threshold

    def verify(self, output_img: image.Image, output_dir: Path, case: RenderCase) -> VerificationResult:
        ref_source = self.get_ref_source()
        if ref_source.has_image_ref():
            ref_img = ref_source.get_image_ref()
            rel_mean = image.re_avg_of(output_img, ref_img)
        else:
            avg_actual = np.average(output_img.values)
            ref_scalar = ref_source.get_ref_scalar()
            rel_mean = (avg_actual - ref_scalar) / ref_scalar

        passed = abs(rel_mean) < self.threshold
        
        return VerificationResult(
            passed=passed,
            message=f"RelMean={rel_mean*100:.3f}% (max={self.threshold*100:.3f}%)" if not passed else "",
            metrics={self.METRIC_REL_MEAN: rel_mean})


class PerPixelVerifier(RefVerifier):
    """
    Performs a strict per-pixel comparison against a constant value.
    """
    METRIC_EXPECTED = "expected"

    def __init__(self, expected_value: float, tolerance: float = 1e-8):
        super().__init__(expected_value)
        self.tolerance = tolerance

    def verify(self, output_img: image.Image, output_dir: Path, case: RenderCase) -> VerificationResult:
        expected_value = self.get_ref_source().get_ref_scalar()
        passed = True
        for value in np.nditer(output_img.values):
            if value != approx(expected_value, abs=self.tolerance):
                passed = False
                break
        
        return VerificationResult(
            passed=passed,
            message=f"Per-pixel check failed (expected {expected_value}, tol={self.tolerance})" if not passed else "",
            metrics={self.METRIC_EXPECTED: expected_value})


class MeanDiffVerifier(RefVerifier):
    """
    Checks image mean against an expected value and stores useful extrema in case debug info.
    """
    METRIC_MEAN_DIFF = "mean_diff"

    def __init__(self, expected_value: float, threshold: float):
        super().__init__(expected_value)
        self.threshold = threshold

    def verify(self, output_img: image.Image, output_dir: Path, case: RenderCase) -> VerificationResult:
        mean_diff = np.mean(output_img.values) - self.get_ref_source().get_ref_scalar()
        case.set_debug_msg("mean diff = %.8f, max pixel = %.8f, min pixel = %.8f" % (
            mean_diff, np.max(output_img.values), np.min(output_img.values)))

        passed = abs(mean_diff) < self.threshold
        return VerificationResult(
            passed=passed,
            message=f"MeanDiff={mean_diff:.8f} (max={self.threshold:.8f})" if not passed else "",
            metrics={self.METRIC_MEAN_DIFF: mean_diff})


class ZTestVerifier(RefVerifier):
    """
    Performs a per-pixel z-test against reference beauty and sample variance images.
    """
    METRIC_SAMPLE_COUNT = "sample_count"
    METRIC_SIGNIFICANCE_LEVEL = "significance_level"
    METRIC_SIDAK_ALPHA = "sidak_alpha"
    METRIC_MIN_P_VALUE = "min_p_value"
    METRIC_PASS_RATIO = "pass_ratio"
    METRIC_REQUIRED_PASS_RATIO = "required_pass_ratio"
    METRIC_VARIANCE_FLOOR = "variance_floor"

    def __init__(
            self,
            ref: RefInput,
            ref_variance: RefInput,
            sample_count: int,
            significance_level: float = 0.0026,
            min_pass_ratio: float = 0.999,
            variance_floor: float = 1e-4):
        """
        @param ref Reference beauty image path or image object.
        @param ref_variance Reference sample variance image path or image object. This must use
        the same dimensions/components as `ref` and `output_img`.
        @param sample_count Number of independent samples used by the tested render.
        @param significance_level Family-wise false-positive probability before Sidak correction.
        @param min_pass_ratio Minimum ratio of scalar image channels whose p-value must pass.
        @param variance_floor Minimum variance used in the denominator to keep near-zero variance
        pixels numerically stable.
        """
        super().__init__(ref)
        self.ref_variance = RefSource(ref_variance)
        self.sample_count = sample_count
        self.significance_level = significance_level
        self.min_pass_ratio = min_pass_ratio
        self.variance_floor = variance_floor
        self._check_params()

    def verify(self, output_img: image.Image, output_dir: Path, case: RenderCase) -> VerificationResult:
        ref_img, ref_var_img = self._get_reference_images(output_img)
        z_stat = self._calculate_z_stat(output_img, ref_img, ref_var_img)
        p_value = self._calculate_p_values(z_stat)
        sidak_alpha = self._calculate_sidak_alpha(p_value.size)
        pass_ratio = self._calculate_pass_ratio(p_value, sidak_alpha)
        min_p_value = np.min(p_value)

        self._save_failure_strength_plot(p_value, sidak_alpha, output_dir, case)
        case.set_debug_msg(self._make_debug_message())
        passed = pass_ratio >= self.min_pass_ratio
        metrics = self._make_metrics(sidak_alpha, min_p_value, pass_ratio)
        message = (
            f"ZTest pass ratio={pass_ratio:.6f} (min={self.min_pass_ratio:.6f}), "
            f"min p-value={min_p_value:.6g}, alpha={sidak_alpha:.6g}") if not passed else ""

        return VerificationResult(passed=passed, message=message, metrics=metrics)

    def _check_params(self):
        if self.sample_count <= 0:
            raise ValueError("sample_count must be positive")
        if not 0.0 < self.significance_level < 1.0:
            raise ValueError("significance_level must be in (0, 1)")
        if not 0.0 <= self.min_pass_ratio <= 1.0:
            raise ValueError("min_pass_ratio must be in [0, 1]")
        if self.variance_floor <= 0.0:
            raise ValueError("variance_floor must be positive")

    def _get_reference_images(self, output_img: image.Image):
        ref_img = self.get_ref_source().get_image_ref()
        ref_var_img = self.ref_variance.get_ref_image_for_output(output_img)
        self._check_dimensions(output_img, ref_img, "reference beauty")
        self._check_dimensions(output_img, ref_var_img, "reference variance")
        return ref_img, ref_var_img

    def _calculate_z_stat(
            self,
            output_img: image.Image,
            ref_img: image.Image,
            ref_var_img: image.Image):
        """
        Calculate `z = |x_bar - mean| / sqrt(s^2 / n)`.
        """
        x_bar = output_img.values
        mean = ref_img.values
        sample_variance = np.maximum(ref_var_img.values, self.variance_floor)
        standard_error = np.sqrt(sample_variance / self.sample_count)
        return np.abs(x_bar - mean) / standard_error

    def _calculate_p_values(self, z_stat):
        """
        Calculate two-sided `p = 2 * (1 - Phi(|z|)) = erfc(|z| / sqrt(2))`.
        """
        return np.vectorize(math.erfc, otypes=[float])(z_stat / math.sqrt(2.0))

    def _calculate_sidak_alpha(self, num_tests: int):
        """
        Calculate Sidak per-test `alpha = 1 - (1 - alpha_family)^(1 / m)`.
        """
        return 1.0 - (1.0 - self.significance_level) ** (1.0 / num_tests)

    def _calculate_pass_ratio(self, p_value, sidak_alpha: float):
        """
        Calculate `count(p_i > alpha) / m`.
        """
        return np.count_nonzero(p_value > sidak_alpha) / p_value.size

    def _make_min_p_value_image(self, p_value):
        """
        Create a single-component image using the smallest channel p-value per pixel.
        """
        p_value_img = image.Image()
        if p_value.ndim == 2:
            p_value_img.values = p_value[:, :, np.newaxis]
        else:
            # Show the weakest channel per pixel, so failures are not hidden by other channels.
            p_value_img.values = p_value.min(axis=2, keepdims=True)
        return p_value_img

    def _make_failure_strength_image(self, p_value, sidak_alpha: float):
        """
        Create a single-component image of `max(0, -log10(p / alpha))`.
        """
        min_p_value = np.maximum(self._make_min_p_value_image(p_value).values, np.finfo(float).tiny)
        failure_strength = np.maximum(0.0, -np.log10(min_p_value / sidak_alpha))
        failure_img = image.Image()
        failure_img.values = failure_strength
        return failure_img

    def _calculate_failure_color_max(self, failure_img: image.Image):
        """
        Calculate a robust color maximum for the Z-test failure-strength plot.
        """
        positive_values = failure_img.values[failure_img.values > 0.0]
        if positive_values.size == 0:
            return 1.0

        return max(1.0, float(np.percentile(positive_values, 95.0)))

    def _save_failure_strength_plot(self, p_value, sidak_alpha: float, output_dir: Path, case: RenderCase):
        failure_img = self._make_failure_strength_image(p_value, sidak_alpha)
        case.set_plot_debug_image_filename(f"{case.get_active_verifier_file_stem()}_failure")
        failure_img.save_pseudocolor_plot(
            output_dir / case.plot_debug_image_filename,
            f"{case.name} Z-Test Failure Strength",
            color_min=0.0,
            color_max=self._calculate_failure_color_max(failure_img),
            color_map='inferno')

    def _make_debug_message(self):
        return (
            "Z-test failure strength = max(0, -log10(p / alpha)). "
            "0 means pass. 1 means the p-value is 10x below Sidak alpha; "
            "2 means 100x below Sidak alpha.")

    def _make_metrics(self, sidak_alpha: float, min_p_value: float, pass_ratio: float):
        return {
            self.METRIC_SAMPLE_COUNT: self.sample_count,
            self.METRIC_SIGNIFICANCE_LEVEL: self.significance_level,
            self.METRIC_SIDAK_ALPHA: sidak_alpha,
            self.METRIC_MIN_P_VALUE: min_p_value,
            self.METRIC_PASS_RATIO: pass_ratio,
            self.METRIC_REQUIRED_PASS_RATIO: self.min_pass_ratio,
            self.METRIC_VARIANCE_FLOOR: self.variance_floor
        }

    def _check_dimensions(self, output_img: image.Image, ref_img: image.Image, ref_name: str):
        if output_img.get_dimensions() != ref_img.get_dimensions():
            raise ValueError(
                f"z-test output and {ref_name} dimensions differ "
                f"({output_img.get_dimensions()} and {ref_img.get_dimensions()})")


class VisualErrorVerifier(RefVerifier):
    """
    Generates scaled error plots for visualization.
    """
    def __init__(
            self, 
            ref: RefInput,
            error_scale: float = 100.0,
            error_output_filename: str = None,
            ref_output_filename: str = None,
            error_title=None,
            ref_title: str = "Reference Image",
            color_max: float = 100.0):
        """
        @param ref Reference image path, image object, or scalar value to compare against.
                   If this is a path or image object, a reference plot is generated for the report.
        @param error_output_filename Custom name for error plot. Defaults to `{case.output_filename}_error`.
        @param ref_output_filename Custom name for reference plot. Defaults to a verifier-specific name.
        @param error_title Optional title for the error plot. May be a callable taking `case`.
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

    def _get_ref_output_filename(self, case: RenderCase):
        return self.ref_output_filename or f"{case.get_active_verifier_file_stem()}_ref"

    def _get_error_output_filename(self, case: RenderCase):
        return self.error_output_filename or f"{case.get_active_verifier_file_stem()}_error"

    def verify(self, output_img: image.Image, output_dir: Path, case: RenderCase) -> VerificationResult:
        error_img = image.Image(output_img.get_width(), output_img.get_height(), output_img.num_components())
        ref_source = self.get_ref_source()

        if ref_source.has_image_ref():
            ref_output_filename = self._get_ref_output_filename(case)
            ref_source.get_image_ref().save_plot(output_dir / ref_output_filename, self.ref_title, create_dirs=True)
            case.set_plot_ref_image_filename(ref_output_filename)
        else:
            case.clear_plot_ref_image_filename()
        case.set_plot_debug_image_filename(self._get_error_output_filename(case))

        if ref_source.has_image_ref():
            ref_img = ref_source.get_image_ref()
            error_img.values = (output_img.values - ref_img.values)
        else:
            error_img.values = (output_img.values - ref_source.get_ref_scalar())

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
        case.set_plot_debug_image_filename(self.output_filename or f"{case.get_active_verifier_file_stem()}_debug")
        debug_img = self.transform(output_img)
        title = self.title(case) if callable(self.title) else (self.title or f"{case.name} Debug Output")
        debug_img.save_pseudocolor_plot(
            output_dir / case.plot_debug_image_filename,
            title,
            color_min=self.color_min,
            color_max=self.color_max,
            color_map=self.color_map)
        return VerificationResult(passed=True)
