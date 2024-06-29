from pathlib import Path


def create_folder(folder_path):
	"""
	Create the folder as specified by `folder_path`. All missing intermediate folders will also be created.
	"""
	path = Path(folder_path)
	path.mkdir(parents = True, exist_ok = True)
