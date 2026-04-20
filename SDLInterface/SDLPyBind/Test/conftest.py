import pytest
import sys
from pathlib import Path

@pytest.fixture(scope="session")
def engine():
    """
    Fixture to import and provide the photon_renderer.engine module.
    Follows the project convention where the CWD is the build/installation root.
    Reference: BlenderAddon/PhotonBlend/psdl/__init__.py
    """
    build_root = Path.cwd()
    
    # Add the build root to sys.path so we can import 'bin.photon_renderer'
    if str(build_root) not in sys.path:
        sys.path.insert(0, str(build_root))
    
    try:
        # Match the import style used in PhotonBlend
        import bin.photon_renderer.engine as engine_module
        return engine_module
    except ImportError as e:
        # Fallback for cases where bin/ might have been added directly to path 
        # or if the execution context differs slightly.
        try:
            import photon_renderer.engine as engine_module
            return engine_module
        except ImportError:
            pytest.exit(f"Could not import photon_renderer.engine. \n"
                        f"Current CWD: {build_root}\n"
                        f"Make sure you are running from the build directory and the module is built.\n"
                        f"Error: {e}")
