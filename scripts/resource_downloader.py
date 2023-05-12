from utility import downloader
from utility import filesystem

import sys
import os


# Download external resources for the renderer
def download_external_resource(dst_directory):

    renderer_resource_folder = os.path.join(dst_directory, "Photon-v2-Resource-main")

    # Delete old resource folder first if it exists (basically a clean install)
    if filesystem.delete_folder_with_contents(renderer_resource_folder):
        print("Old resource folder deleted")

    src_file_url = "https://github.com/TzuChieh/Photon-v2-Resource/archive/main.zip"

    print("Downloading renderer resources...")
    downloader.download_zipfile_and_extract(src_file_url, dst_directory)

    # Check resources
    if os.path.isdir(renderer_resource_folder):
        print("Engine resources saved to <%s>" % renderer_resource_folder)
    else:
        print("Failed to locate engine resources, expected to be <%s>" % renderer_resource_folder, file=sys.stderr)
