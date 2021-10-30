cl.exe /c /EHsc PerspectiveWhiteQuad.cpp

rc.exe Resources.rc

link.exe PerspectiveWhiteQuad.obj Resources.res user32.lib gdi32.lib /SUBSYSTEM:WINDOWS /MACHINE:X64

PerspectiveWhiteQuad
