#!/bin/sh

photon_build_directory=$1
if [ -z "$photon_build_directory" ]
	then 
		echo "Note: build directory not specified, using default"
		photon_build_directory=./build/
fi
echo "build directory set to \"$photon_build_directory\""

python3 "./scripts/download_thirdparty_library.py" "$photon_build_directory"
python3 "./scripts/download_resource.py" "$photon_build_directory"

echo "Setup Completed"