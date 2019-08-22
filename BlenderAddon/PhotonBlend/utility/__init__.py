from mathutils import Vector, Quaternion

import os
from pathlib import Path


def to_photon_vec3(b_vec3):
	return Vector((b_vec3.y, b_vec3.z, b_vec3.x))


def to_photon_quat(b_quat):
	return Quaternion((b_quat.w, b_quat.y, b_quat.z, b_quat.x))


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
	Given a path pointing to a file, return the part pointing to the folder
	where the file sits. A folder path ends with a path separator.
	"""

	folder_path = os.path.split(file_path)[0]
	if not folder_path.endswith(path_separator()):
		folder_path += path_separator()
	return folder_path


def get_appended_path(parent_path, child_path):

	"""
	Appends a path to the other one's back and returns the result as a new
	one. Exactly one path separator will be in-between them.
	"""

	return os.path.join(parent_path, child_path)


def get_absolute_path(path):
	absolute_path = os.path.abspath(path)
	if path.endswith(path_separator()) and not absolute_path.endswith(path_separator()):
		absolute_path += path_separator()
	return absolute_path


def path_separator():
	return os.sep
