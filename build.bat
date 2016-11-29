@echo off

IF NOT EXIST build mkdir build
pushd build
    cl -Zi -nologo ../src/gambling.cpp /link ../lib/sdl2.lib ../lib/sdl2main.lib ../lib/sdl2_ttf.lib -subsystem:console
popd

xcopy /s /y bin build

echo Build finished