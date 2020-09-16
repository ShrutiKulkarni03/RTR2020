cl.exe /c /EHsc ConcentricRectangles.cpp

rc.exe MyIcon.rc

link.exe ConcentricRectangles.obj MyIcon.res user32.lib gdi32.lib /SUBSYSTEM:WINDOWS

ConcentricRectangles