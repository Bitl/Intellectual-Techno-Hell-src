@ECHO off

set vtfnum=1
set mapfile_vmf=""
set mapfile_bsp=""
set sourceproject="C:\Program Files (x86)\Steam\SteamApps\sourcemods\ith"

:compile
set mapfile_vmf="C:\Users\BITL\Documents\GitHub\smod3-Intellectual-Techno-Hell\sp\vmf\ith_level%vtfnum%.vmf"
set mapfile_bsp="C:\Users\BITL\Documents\GitHub\smod3-Intellectual-Techno-Hell\sp\vmf\ith_level%vtfnum%.bsp"
echo Building BSP...
vbsp.exe -vproject %sourceproject% %mapfile_vmf%
echo Embedding visibility data...
vvis.exe -vproject %sourceproject% -fast %mapfile_vmf%
echo Adding lighting...
vrad.exe -vproject %sourceproject% -both %mapfile_bsp%
set /A vtfnum=vtfnum+1
pause
goto compile