@echo off
setlocal

set OBJFOLDER=obj
set OUTFOLDER=bin
set OUTFILE=libphx
set SRCPATH=src\*.cpp

set ARCH=32
set TYPE=dynamic
set INSTALL=1

set ROOT=..
set EXTLIBS=
set EXTLIBS=%EXTLIBS% opengl32.lib glew32.lib
set EXTLIBS=%EXTLIBS% user32.lib winmm.lib Ws2_32.lib
set EXTLIBS=%EXTLIBS% fmodL_vc.lib fmodstudioL_vc.lib
set EXTLIBS=%EXTLIBS% SDL2.lib liblz4.lib freetype.lib lua51.lib tcc.lib
set EXTLIBS=%EXTLIBS% BulletCollision.lib BulletDynamics.lib LinearMath.lib
REM EXTLIBS_R=
REM EXTLIBS_D=
REM EXTLIBS_32=
REM EXTLIBS_64=
REM EXTLIBS_32R=fmod_vc.lib fmodstudio_vc.lib
REM EXTLIBS_32D=fmodL_vc.lib fmodstudioL_vc.lib
REM EXTLIBS_64R=fmod64_vc.lib fmodstudio64_vc.lib
REM EXTLIBS_64D=fmodL64_vc.lib fmodstudioL64_vc.lib
set EXTINCPATHS=%EXTINCPATHS%;%ROOT%/shared/include/bullet/

pushd %~pd0
call %ROOT%\tool\nbsbuild\msvc14.bat %*
if errorlevel 1 exit /B 1
popd

pushd %~pd0\%ROOT%
echo Binding...
set PATH=%PATH%;%~pd0\%ROOT%\shared\lib_win%ARCH%
tool\luajit.exe tool\genffi\genffi.lua "libphx/include" "libphx" "shared/script"
popd
