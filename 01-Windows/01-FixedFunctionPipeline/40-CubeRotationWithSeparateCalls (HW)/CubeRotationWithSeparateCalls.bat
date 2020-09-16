cl.exe /c /EHsc CubeRotationWithSeparateCalls.cpp

rc.exe MyIcon.rc

link.exe CubeRotationWithSeparateCalls.obj MyIcon.res user32.lib gdi32.lib /SUBSYSTEM:WINDOWS

CubeRotationWithSeparateCalls