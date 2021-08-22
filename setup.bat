@ECHO off

SET photon_build_directory=%~1
IF "%photon_build_directory%"=="" (
	ECHO Note: build directory not specified, using default
	SET "photon_build_directory=./build/"
)
ECHO build directory set to "%photon_build_directory%"

md "%photon_build_directory%"

python "./scripts/setup.py" "%photon_build_directory%"

ECHO --- Setup Completed ---
