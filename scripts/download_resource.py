# download zipped resource folder

from utility import downloader

import sys


src_file_url  = "https://drive.google.com/uc?export=download&id=15xh8dKaCpgNTunFZWeaeBhE6Dio3YPgR"
dst_directory = sys.argv[1]

downloader.download_zipfile_and_extract(src_file_url, dst_directory)
