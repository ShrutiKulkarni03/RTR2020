cl.exe /c /EHsc SpinningPyramid.cpp

rc.exe MyIcon.rc

link.exe SpinningPyramid.obj MyIcon.res user32.lib gdi32.lib /SUBSYSTEM:WINDOWS

SpinningPyramid