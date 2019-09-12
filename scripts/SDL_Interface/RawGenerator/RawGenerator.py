from InterfaceGenerator import InterfaceGenerator


class RawGenerator(InterfaceGenerator):
	def generate(self, output_directory):
		file = open(output_directory + "raw_interface.txt", "w+")
		for interface in self.interfaces:
			file.write(str(interface))
		file.close()

	def name(self):
		return "raw"
