cl.exe /c /EHsc Two2DShapes.cpp

rc.exe Resources.rc

link.exe Two2DShapes.obj Resources.res user32.lib gdi32.lib /SUBSYSTEM:WINDOWS /MACHINE:X64

Two2DShapes
