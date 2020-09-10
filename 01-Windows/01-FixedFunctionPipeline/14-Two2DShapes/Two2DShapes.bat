cl.exe /c /EHsc Two2DShapes.cpp

rc.exe MyIcon.res

link.exe Two2DShapes.obj MyIcon.res user32.lib gdi32.lib /SUBSYSTEM:WINDOWS

Two2DShapes