from infra import image
from infra import renderer
from infra import paths

import numpy as np
import pytest


case_name = "white_100W_rect_area_light"

output_images = [
    paths.test_output() / case_name / "bvpt",
    paths.test_output() / case_name / "bneept",
    paths.test_output() / case_name / "sppm",
]

output_debug_images = [
    paths.test_output() / case_name / "bvpt_error",
    paths.test_output() / case_name / "bneept_error",
    paths.test_output() / case_name / "sppm_error",
]

reference_images = [
    paths.test_output() / case_name / "ref"
]

@pytest.fixture(scope='module')
def ref_img():
    img_path = paths.test_resources() / case_name / "ref_bneept_8192spp"
    img = image.read_pfm(img_path)
    img.save_plot(reference_images[0], "Reference: BNEEPT 8192 spp", create_dirs=True)
    return img

def test_render(ref_img):
    """
    A rectangular area light is shining the ground below it. The ground is diffusive (albedo = 50%).
    """
    scenes = [
        paths.test_resources() / case_name / "scene_bvpt.p2",
        paths.test_resources() / case_name / "scene_bneept.p2",
        paths.test_resources() / case_name / "scene_sppm.p2",
    ]

    max_rmses = [0.06, 0.04, 0.12]
    max_re_avgs = [0.0008, 0.0004, 0.002]

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
    