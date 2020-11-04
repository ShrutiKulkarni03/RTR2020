cl.exe /c /EHsc PyramidTexture.cpp

rc.exe MyIcon.rc 

link.exe PyramidTexture.obj MyIcon.res user32.lib gdi32.lib /SUBSYSTEM:WINDOWS

PyramidTexture