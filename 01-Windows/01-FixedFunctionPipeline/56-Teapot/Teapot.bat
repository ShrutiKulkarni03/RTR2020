cl.exe /c /EHsc Teapot.cpp

rc.exe MyIcon.rc 

link.exe Teapot.obj MyIcon.res user32.lib gdi32.lib /SUBSYSTEM:WINDOWS

Teapot