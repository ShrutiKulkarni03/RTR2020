cl.exe /c /EHsc Transformation.cpp

rc.exe MyIcon.rc

link.exe Transformation.obj MyIcon.res user32.lib gdi32.lib /SUBSYSTEM:WINDOWS

Transformation
