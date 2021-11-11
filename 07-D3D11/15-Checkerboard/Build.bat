cl.exe /c /EHsc /MD Checkerboard.cpp

rc.exe Resources.rc

link.exe Checkerboard.obj Resources.res user32.lib gdi32.lib /SUBSYSTEM:WINDOWS /MACHINE:X64

Checkerboard
