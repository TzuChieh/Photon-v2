import sys
import os
import importlib
import datetime

# We are using absolute imports with `PhotonBlend` folder as the root
root_folder_path = os.path.abspath(os.path.dirname(__file__))
print(f"PhotonBlend: using {root_folder_path} as root")

try:
	sys.path.insert(0, root_folder_path)
	from utility import blender
finally:
	sys.path.remove(root_folder_path)

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


# Register all modules (a required Blender callback)
def register():
	main_package_full_name = "{}.{}".format(__name__, "bmodule")
	blender.module_manager = blender.BlenderModuleManager()

	try:
		sys.path.insert(0, root_folder_path)

		# Import or update existing modules
		if main_package_full_name in sys.modules:
			importlib.reload(sys.modules[main_package_full_name])
		else:
			importlib.import_module(main_package_full_name)
	finally:
		sys.path.remove(root_folder_path)

	blender.module_manager.register_all()


# Unregister all modules (a required Blender callback)
def unregister():
	blender.module_manager.unregister_all()
	blender.module_manager = None
