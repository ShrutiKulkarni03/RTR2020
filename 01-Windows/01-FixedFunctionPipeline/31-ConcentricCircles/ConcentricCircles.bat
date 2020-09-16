cl.exe /c /EHsc ConcentricCircles.cpp

rc.exe MyIcon.rc

link.exe ConcentricCircles.obj MyIcon.res user32.lib gdi32.lib /SUBSYSTEM:WINDOWS

ConcentricCircles