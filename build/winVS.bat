@echo off
rem where /q devenv.exe || echo Please install visual studio(https://www.visualstudio.com/products/visual-studio-community-vs) && pause && exit
where /q git.exe || echo Please install command line git(https://git-scm.com/download/win) && pause && exit
if not exist "%PROGRAMFILES(x86)%/CMake/bin/cmake.exe" ( echo Please ensure that cmake is installed(https://cmake.org/download/) && pause && exit )

mkdir win64
cd win64
"%PROGRAMFILES(x86)%/CMake/bin/cmake.exe" -G "Visual Studio 12 win64" ../../Code
"%PROGRAMFILES(x86)%/CMake/bin/cmake.exe" --build .
explorer.exe "%cd%/../../dist/"
pause
