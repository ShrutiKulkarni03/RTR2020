cl.exe /c /EHsc PerspectiveWhiteTriangle.cpp

rc.exe Resources.rc

link.exe PerspectiveWhiteTriangle.obj Resources.res user32.lib gdi32.lib /SUBSYSTEM:WINDOWS /MACHINE:X64

PerspectiveWhiteTriangle
