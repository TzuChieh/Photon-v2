from pathlib import Path


def get_path(rel_resource_path):
    resource_dir = Path(__file__).parent.resolve()
    return resource_dir / rel_resource_path
