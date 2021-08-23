cl.exe /c /EHsc BnWCube.cpp

rc.exe Resources.rc

link.exe BnWCube.obj Resources.res user32.lib gdi32.lib /SUBSYSTEM:WINDOWS /MACHINE:X64

BnWCube
