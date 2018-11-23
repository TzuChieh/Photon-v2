from InterfaceParser import InterfaceParser
from RawGenerator.RawGenerator import RawGenerator

import os
import sys


AVAILABLE_GENERATORS = [
	RawGenerator
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

		with open(file_path, "rt") as source_file:
			source_string = source_file.read()
			parser = InterfaceParser(source_string)
			if parser.has_interface():
				print("interface found in %s" % file_path)
				generator.add_interface(parser.interface)
			# else:
			# 	print("passing %s" % file_path)

output_directory = "./output/"
os.makedirs(os.path.dirname(output_directory), exist_ok=True)

generator.generate(output_directory)

print("interface generated in %s" % output_directory)
