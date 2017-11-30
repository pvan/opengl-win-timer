@echo off

mkdir build

pushd build
cl -Zi ..\app.cpp user32.lib Gdi32.lib Winmm.lib OpenGL32.lib
popd