from pathlib import Path

# FIXME: use importlib.resources for this
def get_path(rel_resource_path):
    resource_dir = Path(__file__).parent.resolve()
    return resource_dir / rel_resource_path
