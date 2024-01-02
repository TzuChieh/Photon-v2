from utility import downloader
from utility import filesystem

import sys
import os


# Download external resources for the renderer
def download_external_resource(dst_directory):

    final_folder_path = os.path.join(dst_directory, "Photon-v2-Resource")

    # Delete old resource folder first if it exists (basically a clean install)
    if filesystem.delete_folder_with_contents(final_folder_path):
        print("Old resource folder deleted")

    src_file_url = "https://github.com/TzuChieh/Photon-v2-Resource/archive/main.zip"

    print("Downloading renderer resources...")
    downloader.download_zipfile_and_extract(src_file_url, dst_directory)
    
    # The extracted zip file will be a folder named "Photon-v2-Resource-<branch-name>",
    # we will rename it to be just "Photon-v2-Resource"
    extracted_folder_path = os.path.join(dst_directory, "Photon-v2-Resource-main")
    filesystem.rename_folder(extracted_folder_path, final_folder_path)

    # Check resources
    if os.path.isdir(final_folder_path):
        print("Renderer resources saved to <%s>" % final_folder_path)
    else:
        print("Failed to locate renderer resources, expected to be <%s>" % final_folder_path, file=sys.stderr)
