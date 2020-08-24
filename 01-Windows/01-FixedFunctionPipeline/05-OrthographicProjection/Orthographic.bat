cl.exe /c /EHsc Orthographic.cpp

rc.exe MyIcon.rc

link.exe Orthographic.obj MyIcon.res user32.lib gdi32.lib /SUBSYSTEM:WINDOWS

Orthographic
