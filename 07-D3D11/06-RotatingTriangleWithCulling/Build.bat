cl.exe /c /EHsc RotatingTriangleWithCulling.cpp

rc.exe Resources.rc

link.exe RotatingTriangleWithCulling.obj Resources.res user32.lib gdi32.lib /SUBSYSTEM:WINDOWS /MACHINE:X64

RotatingTriangleWithCulling
