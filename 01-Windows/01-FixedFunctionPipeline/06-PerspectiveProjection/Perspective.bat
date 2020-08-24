cl.exe /c /EHsc Perspective.cpp

rc.exe MyIcon.rc

link.exe Perspective.obj MyIcon.res user32.lib gdi32.lib /SUBSYSTEM:WINDOWS

Perspective
