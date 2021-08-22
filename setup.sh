#!/bin/sh

photon_build_directory=$1
if [ -z "$photon_build_directory" ]
	then 
		echo "Note: build directory not specified, using default"
		photon_build_directory=./build/
fi
echo "build directory set to \"$photon_build_directory\""

mkdir -p "$photon_build_directory"

python3 "./scripts/setup.py" "$photon_build_directory"

echo "--- Setup Completed ---"
