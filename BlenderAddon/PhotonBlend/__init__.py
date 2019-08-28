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
	"location": "Info Header >> Render Engine Menu",  # FIXME
	"warning": "experimental...",
	"category": "Render"
}

print("PhotonBlend activated. %s" % datetime.datetime.now())

main_package_names = [
	"bmodule.materials",
	"bmodule.lights",
	"bmodule.cameras",
	"bmodule.p2exporter",
	"bmodule.renderer",
	"bmodule.world"
]

main_package_full_names = []
for main_package_name in main_package_names:
	main_package_full_names.append("{}.{}".format(__name__, main_package_name))

module_manager = None


# Register all modules. (A required Blender callback.)
def register():
	module_manager = BlenderModuleManager()

	for main_package_full_name in main_package_full_names:
		# Import or update existing modules.
		if main_package_full_name in sys.modules:
			importlib.reload(sys.modules[main_package_full_name])
		else:
			importlib.import_module(main_package_full_name)

		main_module = sys.modules[main_package_full_name]




	for module_name in main_package_full_names:
		if module_name in sys.modules:
			if hasattr(sys.modules[module_name], "register"):
				sys.modules[module_name].register()
			else:
				print("Blender module %s should contain a register() function" % module_name)
		else:
			print("Blender module %s is not correctly imported" % module_name)


# Unregister all modules. (A required Blender callback.)
def unregister():
	for module_name in main_package_full_names:
		if module_name in sys.modules:
			if hasattr(sys.modules[module_name], "unregister"):
				sys.modules[module_name].unregister()
			else:
				print("Blender module %s should contain an unregister() function" % module_name)
		else:
			print("Blender module %s is not correctly imported" % module_name)
