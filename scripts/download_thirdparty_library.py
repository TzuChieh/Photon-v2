# download zipped folder

import sys
import urllib.request

file_url         = "https://github.com/TzuChieh/Photon-v2-ThirdParty/archive/master.zip"
target_directory = sys.argv[1]
zip_file_path    = "./temp_Library.zip"

def progress_reporter(num_chunks_read, chunk_size, total_size):
    read_so_far = num_chunks_read * chunk_size
    print("\r - Downloaded: %d KB -" % (read_so_far / (1 << 10)), flush = True, end = "")

print("Downloading third-party library %s..." % zip_file_path)
urllib.request.urlretrieve(file_url, zip_file_path, progress_reporter)
print("\nDownload complete.")

# extract zipped folder

print("Extracting file...")

import zipfile
zip_file = zipfile.ZipFile(zip_file_path, "r")
zip_file.extractall(target_directory)
zip_file.close()

# delete zipped folder

print("Deletes temporary file %s." % zip_file_path)

import os
os.remove(zip_file_path)