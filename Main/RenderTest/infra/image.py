import numpy as np
import matplotlib.pyplot as plt

import struct
from pathlib import Path


class Image:
    def __init__(self):
        super().__init__()

        self.values = np.reshape([], (0, 0, 0))

    def get_height(self):
        return self.values.shape[0]

    def get_width(self):
        return self.values.shape[1]
    
    def num_components(self):
        return self.values.shape[2]
    
    def get_dimensions(self):
        return (self.get_height(), self.get_width(), self.num_components())
    
    def to_summed(self):
        img = Image()
        img.values = self.values.sum(axis=2, keepdims=True)
        return img
    
    def to_absolute(self):
        img = Image()
        img.values = np.absolute(self.values)
        return img
    
    def to_summed_absolute(self):
        return self.to_absolute().to_summed()

    def save_plot(self, file_path, title, create_dirs=False):
        if create_dirs:
            Path(file_path).parents[0].mkdir(parents=True, exist_ok=True)

        plt.imshow(self.values, origin='lower', interpolation='nearest')
        plt.title(title)
        plt.savefig(Path(file_path).with_suffix(".png"), bbox_inches='tight')
        plt.clf()

    def save_pseudocolor_plot(self, file_path, title, create_dirs=False):
        if self.num_components() != 1:
            raise ValueError("expected 1 color component, %d were found" % self.num_components())
        
        if create_dirs:
            Path(file_path).parents[0].mkdir(parents=True, exist_ok=True)
        
        plt.imshow(self.values, origin='lower', cmap='nipy_spectral', interpolation='nearest')
        plt.title(title)
        plt.colorbar()
        plt.savefig(Path(file_path).with_suffix(".png"), bbox_inches='tight')
        plt.clf()


def read_pfm(file_path):
    with open(Path(file_path).with_suffix(".pfm"), 'rb') as pfm_file:
        # Parse header

        header_lines = [pfm_file.readline().decode('ascii').strip() for _ in range(3)]
        
        if header_lines[0] == 'PF':
            num_components = 3
        elif header_lines[0] == 'Pf':
            num_components = 1
        else:
            raise ValueError("input is not a .pfm file")
        
        dimensions_str = header_lines[1].split()
        if len(dimensions_str) == 2:
            width = int(dimensions_str[0])
            height = int(dimensions_str[1])
        else:
            raise ValueError("expected two numbers for dimensions, %d were found" % len(dimensions_str))
        
        endian_indicator = float(header_lines[2])
        if endian_indicator < 0:
            endian_format = '<' # little endian
        elif endian_indicator > 0:
            endian_format = '>' # big endian
        else:
            raise ValueError("bad byte order indicator, %f were given" % endian_indicator)
        
        # Parse pixel data

        pixel_bytes = pfm_file.read()
        num_floats = width * height * num_components
        if len(pixel_bytes) != num_floats * 4:
            raise ValueError("expected %d bytes for pixel data, %d bytes were found" % (num_floats * 4, len(pixel_bytes)))

        floats = struct.unpack(endian_format + str(num_floats) + 'f', pixel_bytes)
        shape = (height, width, 3) if num_components == 3 else (height, width)

        image = Image()
        image.values = np.reshape(floats, shape)
        return image

def rmse_of(img_a: Image, img_b: Image):
    dim_a = img_a.get_dimensions()
    dim_b = img_b.get_dimensions()
    if dim_a != dim_b:
        raise ValueError("input images have different dimensions (%s and %s)" % (str(dim_a), str(dim_b)))

    mse = ((img_a.values - img_b.values)**2).mean()
    return np.sqrt(mse)

def re_avg_of(img_actual: Image, img_expected: Image):
    avg_actual = np.average(img_actual.values)
    avg_expected = np.average(img_expected.values)
    return (avg_actual - avg_expected) / avg_expected