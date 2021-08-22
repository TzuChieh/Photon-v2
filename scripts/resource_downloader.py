from utility import downloader

import sys
import os


# Download external resources for the engine
def download_external_resource(dst_directory):

    src_file_url = "https://github.com/TzuChieh/Photon-v2-Resource/archive/main.zip"

    print("Downloading engine resources...")
    downloader.download_zipfile_and_extract(src_file_url, dst_directory)

    # Check resources
    engine_resources_folder = os.path.join(dst_directory, "Photon-v2-Resource-main")
    if os.path.isdir(engine_resources_folder):
        print("Engine resources saved to <%s>" % engine_resources_folder)
    else:
        print("Failed to locate engine resources, expected to be <%s>" % engine_resources_folder, file=sys.stderr)
