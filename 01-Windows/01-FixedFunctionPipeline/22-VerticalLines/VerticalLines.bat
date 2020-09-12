cl.exe /c /EHsc VerticalLines.cpp

rc.exe MyIcon.rc

link.exe VerticalLines.obj MyIcon.res user32.lib gdi32.lib /SUBSYSTEM:WINDOWS

VerticalLines