cl.exe /c /EHsc Light.cpp

rc.exe MyIcon.rc

link.exe Light.obj MyIcon.res user32.lib gdi32.lib /SUBSYSTEM:WINDOWS

Light