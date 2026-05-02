@echo off
setlocal

pushd "%~dp0\.."
python .\RenderTest\report_server.py
popd

endlocal
