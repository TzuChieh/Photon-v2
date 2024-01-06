import infra
from infra import image
from infra import renderer
from infra import paths

import numpy as np
import pytest
from pytest import approx


case = infra.TestCase()
case.name = "fullscreen_unit_radiance"
case.desc = ("These scenes are all arranged in a way that they should render as a frame filled with "
    "white of unit magnitude (1, 1, 1). All output images should appear white, and all images showing "
    "error should be completely black (or any other color representing 0, depending on the color map).")
case.scenes = [
    paths.test_resources() / case.name / "scene_bvpt.p2",
    paths.test_resources() / case.name / "scene_bneept.p2",
    paths.test_resources() / case.name / "scene_sppm.p2",
]
case.outputs = [
    paths.test_output() / case.name / "bvpt",
    paths.test_output() / case.name / "bneept",
    paths.test_output() / case.name / "sppm",
]
case.debug_outputs = [
    paths.test_output() / case.name / "bvpt_error",
    paths.test_output() / case.name / "bneept_error",
    paths.test_output() / case.name / "sppm_error",
]

@pytest.mark.parametrize("scene, output, debug_output", [
    (case.scenes[0], case.outputs[0], case.debug_outputs[0]), 
    (case.scenes[1], case.outputs[1], case.debug_outputs[1]),
    (case.scenes[2], case.outputs[2], case.debug_outputs[2]),
])
def test_render(scene, output, debug_output):
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
        assert value == approx(1.0, abs=1e-8)
    