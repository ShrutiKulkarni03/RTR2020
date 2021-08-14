cl.exe /c /EHsc MulticolorTriangle.cpp

rc.exe Resources.rc

link.exe MulticolorTriangle.obj Resources.res user32.lib gdi32.lib /SUBSYSTEM:WINDOWS /MACHINE:X64

MulticolorTriangle
