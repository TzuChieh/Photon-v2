# download zipped resource folder

import sys
import urllib.request

file_url      = "https://drive.google.com/uc?export=download&id=15xh8dKaCpgNTunFZWeaeBhE6Dio3YPgR"
zip_filename  = "Resource.zip"
zip_file_path = "./" + zip_filename

def progress_reporter(num_chunks_read, chunk_size, total_size):
    read_so_far = num_chunks_read * chunk_size
    print("\r - Downloaded: %d MB -" % (read_so_far / (1 << 20)), flush = True, end = "")

print("Downloading resource file %s to %s..." % (zip_filename, zip_file_path))
urllib.request.urlretrieve(file_url, zip_file_path, progress_reporter)
print("\nDownload complete.")

# extract zipped resource folder

print("Extracting file...")

import zipfile
zip_file = zipfile.ZipFile(zip_file_path, "r")
zip_file.extractall("./")
zip_file.close()

# delete zipped resource file

print("Deletes temporary file.")

import os
os.remove(zip_file_path)