cl.exe /c /EHsc Discipline03.cpp

rc.exe MyIcon.rc

link.exe Discipline03.obj MyIcon.res user32.lib gdi32.lib /SUBSYSTEM:WINDOWS

Discipline03