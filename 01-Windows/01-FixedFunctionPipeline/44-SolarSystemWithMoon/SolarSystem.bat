cl.exe /c /EHsc SolarSystem.cpp

rc.exe MyIcon.rc

link.exe SolarSystem.obj MyIcon.res user32.lib gdi32.lib /SUBSYSTEM:WINDOWS

SolarSystem