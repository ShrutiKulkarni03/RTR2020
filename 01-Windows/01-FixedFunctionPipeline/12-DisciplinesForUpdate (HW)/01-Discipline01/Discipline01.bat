cl.exe /c /EHsc Discipline01.cpp

rc.exe MyIcon.rc

link.exe Discipline01.obj MyIcon.res user32.lib gdi32.lib /SUBSYSTEM:WINDOWS

Discipline01