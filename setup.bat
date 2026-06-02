@ECHO off

REM Create Python virtual environment with setup script first
python -u "./scripts/setup.py" %* --py-env
IF %ERRORLEVEL% NEQ 0 EXIT /B %ERRORLEVEL%

REM Run main setup under virtual environment
CALL "./build/ApplicationEnv/Scripts/activate.bat"

python -u "./scripts/setup.py" %*
SET setup_error=%ERRORLEVEL%

CALL deactivate

IF %setup_error% NEQ 0 EXIT /B %setup_error%
ECHO --- Setup Completed ---
