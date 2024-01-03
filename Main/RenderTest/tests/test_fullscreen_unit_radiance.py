from helpers import image
from helpers import renderer


def test_render():
    process = renderer.open_default_render_process("./scene_bneept.p2", "./hello")
    process.run_and_wait()
    assert True
    