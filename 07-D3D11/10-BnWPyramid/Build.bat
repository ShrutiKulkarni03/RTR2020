cl.exe /c /EHsc BnWPyramid.cpp

rc.exe Resources.rc

link.exe BnWPyramid.obj Resources.res user32.lib gdi32.lib /SUBSYSTEM:WINDOWS /MACHINE:X64

BnWPyramid
