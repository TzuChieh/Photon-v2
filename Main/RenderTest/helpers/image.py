import struct


class Image:
    def __init__(self):
        super.__init__()

        self.width = 0
        self.height = 0
        self.num_components = 0
        self.values = []


def read_pfm(file_path):
    with open(file_path, 'rb') as pfm_file:
        # Parse header

        header_lines = [pfm_file.readline().decode('ascii').strip() for _ in range(3)]
        
        if header_lines[0] == 'PF':
            num_components = 3
        elif header_lines[0] == 'Pf':
            num_components = 1
        else:
            raise Exception("input is not a .pfm file")
        
        dimensions_str = header_lines[1].split()
        if len(dimensions_str) == 2:
            width = int(dimensions_str[0])
            height = int(dimensions_str[1])
        else:
            raise Exception("expected two numbers for dimensions, %d were found" % len(dimensions_str))
        
        endian_indicator = float(header_lines[2])
        if endian_indicator < 0:
            endian_format = '<' # little endian
        elif endian_indicator > 0:
            endian_format = '>' # big endian
        else:
            raise Exception("bad byte order indicator, %f were given" % endian_indicator)
        
        # Parse pixel data

        pixel_bytes = pfm_file.read()
        num_floats = width * height * num_components
        if len(pixel_bytes) != num_floats * 4:
            raise Exception("expected %d bytes for pixel data, %d bytes were found" % (num_floats * 4, len(pixel_bytes)))

        floats = list(struct.unpack(endian_format + str(num_floats) + 'f', pixel_bytes))

        image = Image()
        image.width = width
        image.height = height
        image.num_components = num_components
        image.values = floats

        return image

