cl.exe /c /EHsc MyIcon.cpp

rc.exe MyIcon.rc

link.exe MyIcon.obj MyIcon.res user32.lib gdi32.lib /SUBSYSTEM:WINDOWS

MyIcon
