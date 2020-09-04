cl.exe /c /EHsc Discipline02.cpp

rc.exe MyIcon.rc

link.exe Discipline02.obj MyIcon.res user32.lib gdi32.lib /SUBSYSTEM:WINDOWS

Discipline02