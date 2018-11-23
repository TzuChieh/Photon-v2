from RawGenerator.RawGenerator import RawGenerator

AVAILABLE_GENERATORS = [
	RawGenerator
]

for generator in AVAILABLE_GENERATORS:
	print(generator().name())
