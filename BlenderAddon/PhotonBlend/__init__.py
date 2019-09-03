from .utility.blender import BlenderModuleManager

import sys
import importlib
import datetime

bl_info = {
	"name": "Photon-v2",
	"description": "A renderer featuring physically based rendering.",
	"author": "Tzu-Chieh Chang",
	"version": (2, 0, 0),
	"blender": (2, 80, 0),
	"location": "Render Properties >> Render Engine Menu",
	"warning": "experimental...",
	"category": "Render"
}

print("PhotonBlend activated. %s" % datetime.datetime.now())

main_package_full_name = "{}.{}".format(__name__, "bmodule")
module_manager = None


# Register all modules. (A required Blender callback.)
def register():
	global module_manager
	global main_package_full_name

	module_manager = BlenderModuleManager()

	# Import or update existing modules
	if main_package_full_name in sys.modules:
		importlib.reload(sys.modules[main_package_full_name])
	else:
		importlib.import_module(main_package_full_name)

	# Include modules for further operations
	if main_package_full_name in sys.modules:
		package = sys.modules[main_package_full_name]
		if hasattr(package, "include_module"):
			package.include_module(module_manager)
		else:
			print("Blender package %s should contain a include_module(1) function" % main_package_full_name)
	else:
		print("Blender package %s is not correctly imported" % main_package_full_name)

	module_manager.register_all()


# Unregister all modules. (A required Blender callback.)
def unregister():
	global module_manager

	module_manager.unregister_all()
	module_manager = None
