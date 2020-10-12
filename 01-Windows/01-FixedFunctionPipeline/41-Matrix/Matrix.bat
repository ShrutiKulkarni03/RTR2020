cl.exe /c /EHsc Matrix.cpp

rc.exe MyIcon.rc

link.exe Matrix.obj MyIcon.res user32.lib gdi32.lib /SUBSYSTEM:WINDOWS

Matrix