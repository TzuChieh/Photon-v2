@echo off

rem The argument accepted are as follows:
rem arg1: vs solution directory

set VS_SOLUTION_DIR=%~1
set ENGINE_BUILD_OUTPUT_DIR=%VS_SOLUTION_DIR%Photon-v2_library\Engine\

rem copy engine library include headers
xcopy %VS_SOLUTION_DIR%Engine\Include %ENGINE_BUILD_OUTPUT_DIR%Include /e /y /i /r

rem copy engine library binaries
xcopy %VS_SOLUTION_DIR%x64\Debug\Engine.dll %ENGINE_BUILD_OUTPUT_DIR%Lib\ /e /y /i /r
xcopy %VS_SOLUTION_DIR%x64\Debug\Engine.lib %ENGINE_BUILD_OUTPUT_DIR%Lib\ /e /y /i /r