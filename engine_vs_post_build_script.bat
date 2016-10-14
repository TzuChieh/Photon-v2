@echo off

rem The argument accepted are as follows:
rem arg1: vs solution directory

set VS_SOLUTION_DIR=%~1

xcopy %VS_SOLUTION_DIR%Engine\Include %VS_SOLUTION_DIR%Photon-v2_library\Engine\Include /e /y /i /r
