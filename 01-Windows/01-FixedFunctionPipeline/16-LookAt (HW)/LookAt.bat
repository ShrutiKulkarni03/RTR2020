cl.exe /c /EHsc LookAt.cpp

rc.exe MyIcon.res

link.exe LookAt.obj MyIcon.res user32.lib gdi32.lib /SUBSYSTEM:WINDOWS

LookAt