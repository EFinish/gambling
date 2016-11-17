@echo off

REM call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" x64

IF NOT EXIST build mkdir build
pushd build

cl -Zi ../src/wow.cpp /link ../lib/sdl2.lib ../lib/sdl2main.lib -subsystem:console
popd

echo Build finished