from infra import image
from typing import Any
import threading

class ResourceCache:
    """
    A thread-safe class-level singleton cache for heavy resources like images.
    """
    _name_to_image = {}
    _lock = threading.Lock()

    @staticmethod
    def get_image(name: Any) -> image.Image:
        """
        Load an image from disk or return the cached version.
        @param name Image name. Any object that can be converted to string is accepted.
                    The string form is used as both cache key and image path.
        Thread-safe at the class level.
        """
        image_name = str(name)
        
        with ResourceCache._lock:
            if image_name not in ResourceCache._name_to_image:
                ResourceCache._name_to_image[image_name] = image.read_pfm(image_name)
            return ResourceCache._name_to_image[image_name]
