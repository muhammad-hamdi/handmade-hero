@echo off

for %%I in ("%~dp0..") do set "ROOT=%%~fI"

call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
set "PATH=%ROOT%;%PATH%"

cls
