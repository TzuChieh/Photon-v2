from infra import image
from pathlib import Path
import threading

class ResourceCache:
    """
    A thread-safe class-level singleton cache for heavy resources like images.
    """
    _path_to_image = {}
    _lock = threading.Lock()

    @staticmethod
    def get_image(img_path: Path) -> image.Image:
        """
        Load an image from disk or return the cached version.
        @param img_path Path to the image file (without extension).
        Thread-safe at the class level.
        """
        if not isinstance(img_path, Path):
            raise TypeError("img_path must be pathlib.Path")
        image_path = str(img_path)
        
        with ResourceCache._lock:
            if image_path not in ResourceCache._path_to_image:
                ResourceCache._path_to_image[image_path] = image.read_pfm(img_path)
            return ResourceCache._path_to_image[image_path]
