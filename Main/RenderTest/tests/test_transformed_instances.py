import infra
from infra import paths
import pytest


res_dir = paths.test_resources() / "transformed_instances"
suite = infra.RenderTestSuite(__name__, res_dir)


def output_title(case, verifier_reports):
    return '%s Output (MSE: %f, Delta: %f%%)' % (
        case.name,
        infra.get_metric(verifier_reports, infra.MSEVerifier.METRIC_MSE),
        infra.get_metric(verifier_reports, infra.RelMeanVerifier.METRIC_REL_MEAN) * 100)


translation_ref = res_dir / "ref_translation_only_bneept_4096spp_beauty"
suite.add_case(infra.RenderCase(
    "Translation Only",
    res_dir / "scene_translation_only_bneept.p2",
    infra.RendererConfig(num_threads=4),
    [
        infra.MSEVerifier(ref=translation_ref, threshold=0.042),
        infra.RelMeanVerifier(ref=translation_ref, threshold=0.00085),
        infra.VisualErrorVerifier(
            ref=translation_ref,
            error_scale=100.0,
            ref_title="Reference: BNEEPT 4096 spp")
    ],
    output_filename="translation_only",
    output_title=output_title,
    case_msg="1,024 cuboids form a plane-like surface with random height offsets."))

general_transform_ref = res_dir / "ref_general_transform_bvpt_4096spp_beauty"
suite.add_case(infra.RenderCase(
    "General Transform",
    res_dir / "scene_general_transform_bneept.p2",
    infra.RendererConfig(num_threads=4),
    [
        infra.MSEVerifier(ref=general_transform_ref, threshold=0.015),
        infra.RelMeanVerifier(ref=general_transform_ref, threshold=0.0002),
        infra.VisualErrorVerifier(
            ref=general_transform_ref,
            error_scale=100.0,
            ref_title="Reference: BVPT 4096 spp")
    ],
    output_filename="general_transform",
    output_title=output_title,
    case_msg="1,000 cuboids with random translation, rotation, and scale fill roughly 95% of the frame."))


@pytest.mark.parametrize("case", suite.get_cases(), ids=lambda c: c.name)
def test_render(case):
    """
    Render cuboid instances with translation only or with translation, rotation, and scale under a
    large overhead area light.
    """
    result = infra.TestRunner().run(case)
    assert result.passed, result.message
