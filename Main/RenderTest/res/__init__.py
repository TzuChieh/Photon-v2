import importlib.resources

def get_error_img_bytes():
    ref = importlib.resources.files("res") / "error.jpg"
    with importlib.resources.as_file(ref) as img_path:
        return img_path.read_bytes()


def get_report_template_text():
    ref = importlib.resources.files("res") / "report_template.html"
    with importlib.resources.as_file(ref) as html_path:
        return html_path.read_text(encoding="utf-8")
