cl.exe /c /EHsc Checkerboard.cpp

rc.exe MyIcon.rc 

link.exe Checkerboard.obj MyIcon.res user32.lib gdi32.lib /SUBSYSTEM:WINDOWS

Checkerboard