cl.exe /c /EHsc Point.cpp

rc.exe MyIcon.res

link.exe Point.obj MyIcon.res user32.lib gdi32.lib /SUBSYSTEM:WINDOWS

Point