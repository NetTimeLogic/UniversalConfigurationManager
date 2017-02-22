@echo off
SET script_dir=%~dp0
SET root_dir=..\%script_dir%

set library_name=UCM

echo ========================================================
echo Build
echo ========================================================
set build_dir=%root_dir%\Build
echo BUILD_DIR: %build_dir%
rmdir /q /s %build_dir%
mkdir %build_dir%

call C:\Qt\5.7\mingw53_32\bin\qtenv2.bat

cd %build_dir%
C:\Qt\Static\5.7\bin\qmake %library_dir%\UniversalConfigurationManager_static.pro
C:\Qt\Tools\mingw530_32\bin\mingw32-make -f %build_dir%\Makefile.Release -j32

echo ========================================================
echo Binary
echo ========================================================
set binary_dir=%root_dir%\Binary
echo BINARY_DIR: %binary_dir%

xcopy /y %build_dir%\release\UniversalConfigurationManager.exe %binary_dir%\
