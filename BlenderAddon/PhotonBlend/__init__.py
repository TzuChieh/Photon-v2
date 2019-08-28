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

print("PhotonBlend initializing...")
print(datetime.datetime.now())

root_package_name = "bmodule"

main_package_names = [
	"materials",
	"lights",
	"cameras",
	"p2exporter",
	"renderer",
	"world"
]

main_package_full_names = []
for main_package_name in main_package_names:
	main_package_full_names.append("{}.{}.{}".format(__name__, root_package_name, main_package_name))

for main_package_full_name in main_package_full_names:
	if main_package_full_name in sys.modules:
		importlib.reload(sys.modules[main_package_full_name])
	else:
		importlib.import_module(main_package_full_name)


def register():
	for module_name in main_package_full_names:
		if module_name in sys.modules:
			if hasattr(sys.modules[module_name], "register"):
				sys.modules[module_name].register()
			else:
				print("Blender module %s should contain a register() function" % module_name)
		else:
			print("Blender module %s is not correctly imported" % module_name)


def unregister():
	for module_name in main_package_full_names:
		if module_name in sys.modules:
			if hasattr(sys.modules[module_name], "unregister"):
				sys.modules[module_name].unregister()
			else:
				print("Blender module %s should contain an unregister() function" % module_name)
		else:
			print("Blender module %s is not correctly imported" % module_name)
