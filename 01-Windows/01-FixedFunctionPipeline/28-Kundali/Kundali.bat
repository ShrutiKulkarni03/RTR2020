cl.exe /c /EHsc Kundali.cpp

rc.exe MyIcon.rc

link.exe Kundali.obj MyIcon.res user32.lib gdi32.lib /SUBSYSTEM:WINDOWS

Kundali