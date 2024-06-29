import sys
import os
import importlib
import datetime

# We are using absolute imports with `PhotonBlend` folder as the root
root_folder_path = os.path.abspath(os.path.dirname(__file__))
print("PhotonBlend: using {root_folder_path} as root")
sys.path.append(root_folder_path)

from utility import blender

# Required by Blender: addon header info
bl_info = {
	"name": "Photon-v2",
	"description": "A renderer featuring physically based rendering.",
	"author": "Tzu-Chieh Chang",
	"version": (2, 0, 0),
	"blender": (3, 60, 0),
	"location": "Render Properties >> Render Engine Menu",
	"warning": "experimental...",
	"category": "Render"
}

print(f"PhotonBlend activated. {datetime.datetime.now()}")

main_package_full_name = "{}.{}".format(__name__, "bmodule")


# Register all modules (a required Blender callback)
def register():
	global main_package_full_name

	blender.module_manager = blender.BlenderModuleManager()

	# Import or update existing modules
	if main_package_full_name in sys.modules:
		importlib.reload(sys.modules[main_package_full_name])
	else:
		importlib.import_module(main_package_full_name)

	blender.module_manager.register_all()


# Unregister all modules (a required Blender callback)
def unregister():
	blender.module_manager.unregister_all()
	blender.module_manager = None
