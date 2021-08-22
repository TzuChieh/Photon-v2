import library_downloader
import resource_downloader

import sys


dst_directory = sys.argv[1]

library_downloader.download_thirdparty_library(dst_directory)
resource_downloader.download_external_resource(dst_directory)
