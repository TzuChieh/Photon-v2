# download zipped library folder

from utility import downloader

import sys


src_file_url  = "https://github.com/TzuChieh/Photon-v2-ThirdParty/archive/master.zip"
dst_directory = sys.argv[1]

downloader.download_zipfile_and_extract(src_file_url, dst_directory)
