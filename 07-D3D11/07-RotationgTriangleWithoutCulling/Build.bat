cl.exe /c /EHsc RotationgTriangleWithoutCulling.cpp

rc.exe Resources.rc

link.exe RotationgTriangleWithoutCulling.obj Resources.res user32.lib gdi32.lib /SUBSYSTEM:WINDOWS /MACHINE:X64

RotationgTriangleWithoutCulling
