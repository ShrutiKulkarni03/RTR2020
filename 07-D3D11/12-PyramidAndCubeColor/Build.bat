cl.exe /c /EHsc 3DAnimation.cpp

rc.exe Resources.rc

link.exe 3DAnimation.obj Resources.res user32.lib gdi32.lib /SUBSYSTEM:WINDOWS /MACHINE:X64

3DAnimation
