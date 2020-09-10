cl.exe /c /EHsc 2DAnimation.cpp

rc.exe MyIcon.res

link.exe 2DAnimation.obj MyIcon.res user32.lib gdi32.lib /SUBSYSTEM:WINDOWS

2DAnimation