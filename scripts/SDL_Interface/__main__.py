from InterfaceParser import InterfaceParser
from RawGenerator.RawGenerator import RawGenerator
from PythonGenerator.PythonGenerator import PythonGenerator
from JavaGenerator.JavaGenerator import JavaGenerator

import os
import sys


AVAILABLE_GENERATORS = [
	RawGenerator,
	PythonGenerator,
	JavaGenerator
]

print("available generators: ")
for gen in AVAILABLE_GENERATORS:
	print(gen().name())

name = input("pick a generator: ")
generator = None
for gen in AVAILABLE_GENERATORS:
	if name == gen().name():
		generator = gen()
		break

if generator is None:
	print("warning: cannot find generator %s" % name)
	sys.exit()

src_directory = input("source directory: ")

for folder, sub_folders, files in os.walk(src_directory):
	for filename in files:
		file_path = os.path.join(folder, filename)
		if file_path.endswith(".exe"):
			continue

		print("processing %s" % file_path)
		with open(file_path, "rt", encoding="utf-8") as source_file:
			source_string = source_file.read()
			parser = InterfaceParser(source_string)
			if parser.has_interface():
				print("interface found in %s" % file_path)
				generator.add_interface(parser.interface)

output_directory = "./output/"
os.makedirs(os.path.dirname(output_directory), exist_ok=True)

generator.resolve_interface_extension()
generator.generate(output_directory)

print("interface generated in %s" % output_directory)
