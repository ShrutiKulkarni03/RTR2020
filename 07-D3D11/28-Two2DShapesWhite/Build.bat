cl.exe /c /EHsc Two2DShapesWhite.cpp

rc.exe Resources.rc

link.exe Two2DShapesWhite.obj Resources.res user32.lib gdi32.lib /SUBSYSTEM:WINDOWS /MACHINE:X64

Two2DShapesWhite
