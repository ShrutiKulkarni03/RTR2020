cl.exe /c /EHsc Materials.cpp

rc.exe Resources.rc

link.exe Materials.obj Resources.res user32.lib gdi32.lib /SUBSYSTEM:WINDOWS 

Materials