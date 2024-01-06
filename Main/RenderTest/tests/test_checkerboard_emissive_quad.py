from infra import image
from infra import renderer
from infra import paths

import numpy as np
import pytest


case_name = "checkerboard_emissive_quad"

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
    img_path = paths.test_resources() / case_name / "ref_bvpt_8192spp"
    img = image.read_pfm(img_path)
    img.save_plot(reference_images[0], "Reference: BVPT 8192 spp", create_dirs=True)
    return img

def test_render(ref_img):
    """
    A emissive quad is placed in front of the camera. The emission is textured with a checkerboard image.
    The quad will perfectly fit the rendered image (i.e., the rendered output should be identical to the
    checkboard image, with a different resolution).
    """
    scenes = [
        paths.test_resources() / case_name / "scene_bvpt.p2",
        paths.test_resources() / case_name / "scene_bneept.p2",
        paths.test_resources() / case_name / "scene_sppm.p2",
    ]

    for scene, output, debug_output in zip(scenes, output_images, output_debug_images):
        process = renderer.open_default_render_process(scene, output, num_threads=6)
        process.run_and_wait()

        output_img = image.read_pfm(output)
        rmse = image.rmse_of(output_img, ref_img)
        output_img.save_plot(output, output.name.upper() + " Output (RMSE: %f)" % rmse)

        output_img.values -= ref_img.values
        output_img.values *= 100
        output_img = output_img.to_summed_absolute()
        output_img.save_pseudocolor_plot(debug_output, output.name.upper() + " 100X Absolute Error")

        assert rmse < 0.01
    