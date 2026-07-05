import infra
from infra import paths
import pytest


res_dir = paths.test_resources() / "normal_mapped_plane"
suite = infra.RenderTestSuite(__name__, res_dir)

renderer_config = infra.RendererConfig(num_threads=4)
pt_ref_path = res_dir / "ref_bneept_32768spp_beauty"
pppm_ref_path = res_dir / "ref_pppm_8192passes_beauty"
bvpt_ref_var_path = res_dir / "ref_bvpt_32768spp_var"
bneept_ref_var_path = res_dir / "ref_bneept_32768spp_var"


def output_title(case, verifier_reports):
    return "%s Output (MSE: %f, Delta: %f%%)" % (
        case.name,
        infra.get_metric(verifier_reports, infra.MSEVerifier.METRIC_MSE),
        infra.get_metric(verifier_reports, infra.RelMeanVerifier.METRIC_REL_MEAN) * 100)


for case_name, output_name, scene_name, ref_path, ref_title, ref_var_path, z_sample_count, max_mse, max_rel_mean in [
    ("BVPT", "bvpt", "scene_bvpt.p2", pt_ref_path, "Reference: BNEEPT 32768 spp", bvpt_ref_var_path, 3072, 0.0058, 0.003),
    ("BNEEPT", "bneept", "scene_bneept.p2", pt_ref_path, "Reference: BNEEPT 32768 spp", bneept_ref_var_path, 1024, 0.003, 0.003),
    ("BNEEPT DirectX", "bneept_directx", "scene_bneept_directx.p2", pt_ref_path, "Reference: BNEEPT 32768 spp", bneept_ref_var_path, 1024, 0.003, 0.003),
    ("PPPM", "pppm", "scene_pppm.p2", pppm_ref_path, "Reference: PPPM 8192 passes", None, None, 0.0013, 0.005)
    ]:
    verifiers = [
        infra.MSEVerifier(ref=ref_path, threshold=max_mse),
        infra.RelMeanVerifier(ref=ref_path, threshold=max_rel_mean),
        infra.VisualErrorVerifier(
            ref=ref_path,
            error_scale=100.0,
            ref_title=ref_title)]
    if z_sample_count is not None:
        verifiers.append(infra.ZTestVerifier(
            ref=ref_path,
            ref_variance=ref_var_path,
            sample_count=z_sample_count))

    suite.add_case(infra.RenderCase(
        case_name,
        res_dir / scene_name,
        renderer_config,
        verifiers,
        output_filename=output_name,
        output_title=output_title))


@pytest.mark.parametrize("case", suite.get_cases(), ids=lambda c: c.name)
def test_render(case):
    """
    A front-facing square normal-mapped wall is lit from above by a large
    rectangular area light, like a gallery wall display.
    """
    runner = infra.TestRunner()
    result = runner.run(case)
    assert result.passed, result.message
