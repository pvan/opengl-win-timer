@echo off


REM change to path of vcvarsall.bat as needed

if not defined DevEnvDir (
    call "D:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
)


IF NOT EXIST build mkdir build

pushd build
cl -Zi ..\app.cpp user32.lib Gdi32.lib Winmm.lib OpenGL32.lib
popd