cl.exe /c /EHsc Viewports.cpp

rc.exe MyIcon.rc

link.exe Viewports.obj MyIcon.res user32.lib gdi32.lib /SUBSYSTEM:WINDOWS

Viewports