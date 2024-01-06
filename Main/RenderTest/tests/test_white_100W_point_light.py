from infra import image
from infra import renderer
from infra import paths

import numpy as np
import pytest


case_name = "white_100W_point_light"

output_images = [
    paths.test_output() / case_name / "bneept",
    paths.test_output() / case_name / "sppm",
]

output_debug_images = [
    paths.test_output() / case_name / "bneept_error",
    paths.test_output() / case_name / "sppm_error",
]

reference_images = [
    paths.test_output() / case_name / "ref"
]

@pytest.fixture(scope='module')
def ref_img():
    img_path = paths.test_resources() / case_name / "ref_bneept_65536spp"
    img = image.read_pfm(img_path)
    img.save_plot(reference_images[0], "Reference: BNEEPT 65536 spp", create_dirs=True)
    return img

def test_render(ref_img):
    """
    This test is similar to the \"white_100W_small_rect_area_light\" test, except that we are using a point
    light here. In Photon, we do not have true point light, they are just tiny spherical light. This test
    is not suitable to run using BVPT, as it may require way more than 10M samples to have proper convergence.
    """
    scenes = [
        paths.test_resources() / case_name / "scene_bneept.p2",
        paths.test_resources() / case_name / "scene_sppm.p2",
    ]

    max_rmses = [0.006, 0.013]
    max_re_avgs = [0.001, 0.0024]

    for scene, output, debug_output, max_rmse, max_re_avg in zip(scenes, output_images, output_debug_images, max_rmses, max_re_avgs):
        process = renderer.open_default_render_process(scene, output, num_threads=6)
        process.run_and_wait()

        output_img = image.read_pfm(output)
        rmse = image.rmse_of(output_img, ref_img)
        re_avg = image.re_avg_of(output_img, ref_img)
        output_img.save_plot(output, output.name.upper() + " Output (RMSE: %f, Δ: %f%%)" % (rmse, re_avg * 100))

        output_img.values -= ref_img.values
        output_img.values *= 100
        output_img = output_img.to_summed_absolute()
        output_img.save_pseudocolor_plot(debug_output, output.name.upper() + " 100X Absolute Error")

        assert rmse < max_rmse
        assert abs(re_avg) < max_re_avg
    