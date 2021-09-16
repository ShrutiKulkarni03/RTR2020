cl.exe /c /EHsc BnWSphere.cpp

rc.exe Resources.rc

link.exe BnWSphere.obj Resources.res user32.lib gdi32.lib /SUBSYSTEM:WINDOWS 

BnWSphere
