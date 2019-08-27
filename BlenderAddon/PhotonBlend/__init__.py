import sys
import importlib
import datetime

bl_info = {
	"name":        "Photon-v2",
	"description": "A hobby renderer featuring PBR.",
	"author":      "Tzu-Chieh Chang",
	"version":     (2, 0, 0),
	"blender":     (2, 80, 0),
	"location":    "Info Header >> Render Engine Menu",
	"warning":     "experimental...",  # showing warning icon and text in addons panel
	"category":    "Render"
}

print("PhotonBlend initializing...")
print(datetime.datetime.now())

bmodulePackageName = "bmodule"
bmoduleNames       = [
	"p2exporter",
	"matl",
	"renderer",
	"light",
	"node",
	"world",
	"camera"
]

bmoduleFullNames = []
for bmoduleName in bmoduleNames:
	bmoduleFullNames.append("{}.{}.{}".format(__name__, bmodulePackageName, bmoduleName))

for bmoduleFullName in bmoduleFullNames:
	if bmoduleFullName in sys.modules:
		importlib.reload(sys.modules[bmoduleFullName])
	else:
		importlib.import_module(bmoduleFullName)


def register():
	for moduleName in bmoduleFullNames:
		if moduleName in sys.modules:
			if hasattr(sys.modules[moduleName], "register"):
				sys.modules[moduleName].register()
			else:
				print("bmodule %s should contain a register() function" % moduleName)
		else:
			print("bmodule %s is not correctly imported" % moduleName)


def unregister():
	for moduleName in bmoduleFullNames:
		if moduleName in sys.modules:
			if hasattr(sys.modules[moduleName], "unregister"):
				sys.modules[moduleName].unregister()
			else:
				print("bmodule %s should contain an unregister() function" % moduleName)
		else:
			print("bmodule %s is not correctly imported" % moduleName)

