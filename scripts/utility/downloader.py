import urllib.request
import zipfile
import os
import time


def download_file(src_file_url, dst_file_path, max_retry=10):

	print("Downloading file %s..." % dst_file_path)

	def progress_reporter(num_chunks_read, chunk_size, total_size):
		read_so_far = num_chunks_read * chunk_size
		print("\r - Downloaded: %d MB -" % (read_so_far / (1 << 20)), flush=True, end="")

	num_retry = 0
	while True:
		try:
			urllib.request.urlretrieve(src_file_url, dst_file_path, progress_reporter)
		except Exception as e:
			print(e)
			if num_retry < max_retry:
				print("Error downloading file, remaining trials: %d" % (max_retry - num_retry))
				print("Download will restart after 30 secs")
				time.sleep(30)
				num_retry = num_retry + 1
				continue
			else:
				print("Download failed.")
				break
		else:
			print("\nDownload completed.")
			break


def download_zipfile_and_extract(src_zipfile_url, dst_directory):

	temp_dst_file_path = os.path.join(dst_directory, "__temp__" + str(time.time()))
	download_file(src_zipfile_url, temp_dst_file_path)

	print("Extracting file...")

	# Extract zipped resource folder
	zip_file = zipfile.ZipFile(temp_dst_file_path, "r")
	zip_file.extractall(dst_directory)
	zip_file.close()

	print("Deleting temporary file %s." % temp_dst_file_path)

	# Delete zipped resource file
	os.remove(temp_dst_file_path)

