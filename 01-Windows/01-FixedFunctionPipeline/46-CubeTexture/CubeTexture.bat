cl.exe /c /EHsc CubeTexture.cpp

rc.exe MyIcon.rc 

link.exe CubeTexture.obj MyIcon.res user32.lib gdi32.lib /SUBSYSTEM:WINDOWS

CubeTexture