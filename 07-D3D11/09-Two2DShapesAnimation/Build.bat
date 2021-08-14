cl.exe /c /EHsc Two2DShapesAnimation.cpp

rc.exe Resources.rc

link.exe Two2DShapesAnimation.obj Resources.res user32.lib gdi32.lib /SUBSYSTEM:WINDOWS /MACHINE:X64

Two2DShapesAnimation
