import importlib.resources

def get_error_img_bytes():
    ref = importlib.resources.files("res") / "error.jpg"
    with importlib.resources.as_file(ref) as img_path:
        return img_path.read_bytes()
