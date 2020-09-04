cl.exe /c /EHsc SpinningTriangle.cpp

rc.exe MyIcon.rc

link.exe SpinningTriangle.obj MyIcon.res user32.lib gdi32.lib /SUBSYSTEM:WINDOWS

SpinningTriangle
