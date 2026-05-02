import infra
from infra import paths
import pytest


res_dir = paths.test_resources() / "cornell_box_with_gold_sphere"
suite = infra.RenderTestSuite(__name__, res_dir)

num_threads_to_test = [1, 2, 5, 8, 17, 64, 100]
ref_path = res_dir / "ref_bneept_32768spp"

def output_title(case, metrics):
    return "%s Output (MSE: %f, Δ: %f%%)" % (case.name, metrics["mse"], metrics["rel_mean"] * 100)

visual_error_verifier = infra.VisualErrorVerifier(
    ref=ref_path,
    error_scale=100.0,
    ref_title="Reference: BNEEPT 32768 spp")

for i, num_threads in enumerate(num_threads_to_test):
    suite.add_case(infra.RenderCase(
        "BNEEPT (%d) %dT" % (i + 1, num_threads),
        res_dir / "scene_bneept.p2",
        infra.RendererConfig(num_threads=num_threads),
        [
            infra.MSEVerifier(ref=ref_path, threshold=0.0016),
            infra.RelMeanVerifier(ref=ref_path, threshold=0.005),
            visual_error_verifier
        ],
        output_filename="bneept_%dt" % num_threads,
        output_title=output_title))

@pytest.mark.parametrize("case", suite.get_cases(), ids=lambda c: c.name)
def test_render(case):
    """
    This test is for testing threaded rendering on a simple scene.
    """
    runner = infra.TestRunner()
    result = runner.run(case)
    assert result.passed, result.message
