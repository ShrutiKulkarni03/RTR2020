cl.exe /c /EHsc PerspectiveBlueQuad_2.cpp

rc.exe Resources.rc

link.exe PerspectiveBlueQuad_2.obj Resources.res user32.lib gdi32.lib /SUBSYSTEM:WINDOWS /MACHINE:X64

PerspectiveBlueQuad_2
