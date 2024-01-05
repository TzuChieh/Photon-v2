from infra import image
from infra import renderer
from infra import paths

import numpy as np
from pytest import approx


case_name = "white_100W_small_rect_area_light"

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

def test_render():
    """
    This test is similar to the \"white_100W_rect_area_light\" test, except that the rectangular area light
    is much smaller (0.005 x 0.005 unit^2), and the light source is placed higher above the ground so it
    cannot induce too much variance (it is a strong radiance source). This test is not suitable to run using
    BVPT, as it may require way more than 10M samples to have proper convergence.
    """
    ref_img_path = paths.test_resources() / case_name / "ref_bneept_4096spp"
    ref_img = image.read_pfm(ref_img_path)
    ref_img.save_plot(reference_images[0], "Reference: BNEEPT 4096 spp", create_dirs=True)
    
    scenes = [
        paths.test_resources() / case_name / "scene_bneept.p2",
        paths.test_resources() / case_name / "scene_sppm.p2",
    ]

    max_rmses = [0.002, 0.05]
    max_re_avgs = [0.00004, 0.007]

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
    