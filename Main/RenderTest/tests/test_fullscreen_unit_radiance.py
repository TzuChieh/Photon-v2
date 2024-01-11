import infra
from infra import image
from infra import renderer
from infra import paths

import numpy as np
import pytest
from pytest import approx


res_dir = paths.test_resources() / "fullscreen_unit_radiance"

bvpt_case = infra.TestCase(__name__, "BVPT", res_dir / "scene_bvpt.p2")
bvpt_case.output = "bvpt"
bvpt_case.debug_output = "bvpt_error"

bneept_case = infra.TestCase(__name__, "BNEEPT", res_dir / "scene_bneept.p2")
bneept_case.output = "bneept"
bneept_case.debug_output = "bneept_error"

sppm_case = infra.TestCase(__name__, "SPPM", res_dir / "scene_sppm.p2")
sppm_case.output = "sppm"
sppm_case.debug_output = "sppm_error"

@pytest.mark.parametrize("case", [
    pytest.param(bvpt_case, id=bvpt_case.get_name()),
    pytest.param(bneept_case, id=bneept_case.get_name()),
    pytest.param(sppm_case, id=sppm_case.get_name()),
])
def test_render(case):
    """
    These scenes are all arranged in a way that they should render as a frame filled with
    white of unit magnitude (1, 1, 1). All output images should appear white, and all images showing
    error should be completely black (or any other color representing 0, depending on the color map).
    """
    process = renderer.open_default_render_process(case.get_scene_path(), case.get_output_path())
    process.run_and_wait()

    img = image.read_pfm(case.get_output_path())
    img.save_plot(case.get_output_path(), case.get_name() + " Output")

    img.values -= 1.0
    img.values *= 100
    img = img.to_summed_absolute_components()
    img.save_pseudocolor_plot(case.get_debug_output_path(), case.get_name() + " 100X Absolute Error")

    img = image.read_pfm(case.get_output_path())
    for value in np.nditer(img.values):
        assert value == approx(1.0, abs=1e-8)
    