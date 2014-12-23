@echo off
set PATH=%~dp0\..\mingw32\bin;%PATH%
cd ..\libbpg-*
mingw32-make.exe libbpg.a
copy libbpg.a ..\
copy libbpg.h ..\
set /p CONFIG_BPG_VERSION=<VERSION
cd %~dp0

set CPATH=%~dp0\..
set LIBRARY_PATH=%~dp0\..
mingw32-make.exe
zip ifBPG-%CONFIG_BPG_VERSION%.zip ifBPG.spi

cd ..\
del libbpg.a libbpg.h

echo end
pause

