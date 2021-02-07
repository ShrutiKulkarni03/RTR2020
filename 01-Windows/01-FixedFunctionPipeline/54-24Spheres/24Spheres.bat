cl.exe /c /EHsc 24Spheres.cpp

rc.exe MyIcon.rc

link.exe 24Spheres.obj MyIcon.res user32.lib gdi32.lib /SUBSYSTEM:WINDOWS

24Spheres
