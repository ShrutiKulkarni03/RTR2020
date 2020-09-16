cl.exe /c /EHsc PyramindAndCube.cpp

rc.exe MyIcon.rc

link.exe PyramindAndCube.obj MyIcon.res user32.lib gdi32.lib /SUBSYSTEM:WINDOWS

PyramindAndCube