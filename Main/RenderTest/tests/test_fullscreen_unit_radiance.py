from infra import image
from infra import renderer
from infra import paths

import numpy as np
from pytest import approx


case_name = "fullscreen_unit_radiance"

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

def test_render():
    """
    The scenes are all arranged in a way that they should render as a frame filled with white of unit magnitude
    (1, 1, 1). All output images should appear white, and all images showing error should be completely black
    (or any other color representing 0, depending on the color map).
    """
    scenes = [
        paths.test_resources() / case_name / "scene_bvpt.p2",
        paths.test_resources() / case_name / "scene_bneept.p2",
        paths.test_resources() / case_name / "scene_sppm.p2",
    ]

    for scene, output, debug_output in zip(scenes, output_images, output_debug_images):
        process = renderer.open_default_render_process(scene, output)
        process.run_and_wait()

        img = image.read_pfm(output)
        img.save_plot(output, output.name.upper() + " Output")

        img.values -= 1.0
        img.values *= 100
        img = img.to_summed_absolute()
        img.save_pseudocolor_plot(debug_output, output.name.upper() + " 100X Absolute Error")

        img = image.read_pfm(output)
        for value in np.nditer(img.values):
            assert value == approx(1.0, abs=1e-6)
    