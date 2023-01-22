import mathutils
from mathutils import Vector, Quaternion, Matrix

import os
import math
from pathlib import Path


def blender_to_photon_quat():
	"""
	In Blender, its coordinate system is right-handed, x-right, z-up and y-front. This function returns
	a `Quaternion` to transform from Blender's to Photon's coordinate system (which is right-handed, x-right, 
	y-up and -z-front).
	"""
	return mathutils.Quaternion((1.0, 0.0, 0.0), math.radians(-90.0))


def blender_to_photon_mat():
	"""
	Returns a 3x3 `Matrix` to transform from Blender's to Photon's coordinate system.
	"""
	# TODO: could be a constant (directly specify matrix elements)
	return blender_to_photon_quat().to_matrix()


def to_photon_vec3(b_vec3: Vector):
	"""
	Transform a `Vector` from Blender's to Photon's coordinate system.
	"""
	return blender_to_photon_mat() @ b_vec3


def to_photon_quat(b_quat: Quaternion):
	"""
	Transform a `Quaternion` from Blender's to Photon's coordinate system. Keep in mind that Blender's 
	`Quaternion` is in (w, x, y, z) order, and the `@` operator do not work like `Matrix` 
	(its `quat @ additional_quat` while matrix is `additional_mat @ mat`).
	"""
	return b_quat @ blender_to_photon_quat()


def to_photon_pos_rot_scale(b_matrix: Matrix):
	"""
	In Blender, its coordinate system is right-handed, x-right, z-up and y-front. This function takes a 
	matrix from Blender and returns decomposed transformations (position, rotation and scale) in Photon's 
	coordinate system (which is right-handed, x-right, y-up and -z-front).
	"""
	blender_to_photon = blender_to_photon_mat().to_4x4()

	pos, rot, scale = (blender_to_photon @ b_matrix).decompose()

	return pos, rot, scale


def create_folder(folder_path):
	path = Path(folder_path)
	path.mkdir(parents = True, exist_ok = True)


def get_filename(file_path):
	return os.path.basename(file_path)


def get_filename_without_ext(file_path):
	filename = os.path.basename(file_path)
	return os.path.splitext(filename)[0]


def get_folder_path(file_path):
	"""
	Given a path pointing to a file, return the part pointing to the folder where the file sits. A folder path ends
	with a path separator.
	"""

	folder_path = os.path.split(file_path)[0]
	if not folder_path.endswith(path_separator()):
		folder_path += path_separator()
	return folder_path


def get_appended_path(parent_path, child_path):
	"""
	Appends a path to the other one's back and returns the result as a new one. Exactly one path separator will be
	in-between them.
	"""

	return os.path.join(parent_path, child_path)


def get_absolute_path(path):
	absolute_path = os.path.abspath(path)
	if path.endswith(path_separator()) and not absolute_path.endswith(path_separator()):
		absolute_path += path_separator()
	return absolute_path


def path_separator():
	return os.sep
