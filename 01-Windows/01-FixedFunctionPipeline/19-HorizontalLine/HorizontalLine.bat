cl.exe /c /EHsc HorizontalLine.cpp

rc.exe MyIcon.rc

link.exe HorizontalLine.obj MyIcon.res user32.lib gdi32.lib /SUBSYSTEM:WINDOWS

HorizontalLine