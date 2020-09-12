cl.exe /c /EHsc VerticalLine.cpp

rc.exe MyIcon.rc

link.exe VerticalLine.obj MyIcon.res user32.lib gdi32.lib /SUBSYSTEM:WINDOWS

VerticalLine