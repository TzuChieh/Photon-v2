# download zipped resource folder

from utility import downloader

import sys


src_file_url  = "https://github.com/TzuChieh/Photon-v2-Resource/archive/main.zip"
dst_directory = sys.argv[1]

print("Downloading resources...")
downloader.download_zipfile_and_extract(src_file_url, dst_directory)
