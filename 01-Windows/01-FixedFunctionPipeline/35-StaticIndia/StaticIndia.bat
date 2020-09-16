cl.exe /c /EHsc StaticIndia.cpp

rc.exe MyIcon.rc

link.exe StaticIndia.obj MyIcon.res user32.lib gdi32.lib /SUBSYSTEM:WINDOWS

StaticIndia