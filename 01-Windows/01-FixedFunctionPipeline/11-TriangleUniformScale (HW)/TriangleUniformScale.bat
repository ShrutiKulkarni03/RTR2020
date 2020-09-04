cl.exe /c /EHsc TriangleUniformScale.cpp

rc.exe MyIcon.rc

link.exe TriangleUniformScale.obj MyIcon.res user32.lib gdi32.lib /SUBSYSTEM:WINDOWS

TriangleUniformScale