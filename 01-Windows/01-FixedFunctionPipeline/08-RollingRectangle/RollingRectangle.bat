cl.exe /c /EHsc RollingRectangle.cpp

rc.exe MyIcon.rc

link.exe RollingRectangle.obj MyIcon.res user32.lib gdi32.lib /SUBSYSTEM:WINDOWS

RollingRectangle
