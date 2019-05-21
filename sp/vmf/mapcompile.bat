@ECHO off

set mapfile_vmf="C:\Users\BITL\Documents\GitHub\smod3-Intellectual-Techno-Hell\sp\vmf\ith_level35.vmf"
set mapfile_bsp="C:\Users\BITL\Documents\GitHub\smod3-Intellectual-Techno-Hell\sp\vmf\ith_level35.bsp"
set sourceproject="C:\Program Files (x86)\Steam\SteamApps\sourcemods\ith"

echo Building BSP...
vbsp.exe -vproject %sourceproject% %mapfile_vmf%
echo Embedding visibility data...
vvis.exe -vproject %sourceproject% -fast %mapfile_vmf%
echo Adding lighting...
vrad.exe -vproject %sourceproject% -both %mapfile_bsp%
pause