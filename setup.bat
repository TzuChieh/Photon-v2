@ECHO off

REM Create Python virtual environment with setup script first
python -u "./scripts/setup.py" %* --py-env

REM Run main setup under virtual environment
IF %ERRORLEVEL% EQU 0 (
    CALL "./build/ApplicationEnv/Scripts/activate.bat"
    python -u "./scripts/setup.py" %*
    CALL deactivate
)

ECHO --- Setup Completed ---
