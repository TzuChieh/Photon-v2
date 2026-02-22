from generated import pysdl as sdl
from psdl.sdlconsole import SdlConsole

import bpy

import sys
import importlib


_cached_photon_renderer_pybind_module = None

def _get_photon_renderer_pybind_module():
    installation_dir = bpy.context.preferences.addons['PhotonBlend'].preferences.installation_path
    if not installation_dir:
        raise ValueError("Please set installation directory.")

    try:
        sys.path.insert(0, installation_dir)
        return importlib.import_module('bin.photon_renderer')
    finally:
        sys.path.remove(installation_dir)


def direct():
    """
    @return Obtain the module for PSDL direct execution.
    """
    global _cached_photon_renderer_pybind_module

    if _cached_photon_renderer_pybind_module is None:
        _cached_photon_renderer_pybind_module = _get_photon_renderer_pybind_module()

    return _cached_photon_renderer_pybind_module
