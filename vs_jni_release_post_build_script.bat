@echo off

rem The argument accepted are as follows:
rem arg1: vs solution directory

set VS_SOLUTION_DIR=%~1
set JNI_BUILD_OUTPUT_DIR=%VS_SOLUTION_DIR%Photon-v2_library\JNI\

rem copy JNI library binaries
xcopy %VS_SOLUTION_DIR%x64\Release\JNI.dll %JNI_BUILD_OUTPUT_DIR% /e /y /i /r
xcopy %VS_SOLUTION_DIR%x64\Release\JNI.lib %JNI_BUILD_OUTPUT_DIR% /e /y /i /r