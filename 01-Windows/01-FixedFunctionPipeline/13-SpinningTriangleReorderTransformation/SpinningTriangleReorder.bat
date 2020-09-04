cl.exe /c /EHsc SpinningTriangleReorder.cpp

rc.exe MyIcon.rc

link.exe SpinningTriangleReorder.obj MyIcon.res user32.lib gdi32.lib /SUBSYSTEM:WINDOWS

SpinningTriangleReorder
