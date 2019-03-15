# download zipped resource folder

from utility import downloader

import sys


src_file_url  = "http://www.cmlab.csie.ntu.edu.tw/~aa0189306/Resource.zip"
dst_directory = sys.argv[1]

print("Downloading resources...")
downloader.download_zipfile_and_extract(src_file_url, dst_directory)
