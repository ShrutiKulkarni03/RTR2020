cl.exe /c /EHsc ConcentricTriangles.cpp

rc.exe MyIcon.rc

link.exe ConcentricTriangles.obj MyIcon.res user32.lib gdi32.lib /SUBSYSTEM:WINDOWS

ConcentricTriangles