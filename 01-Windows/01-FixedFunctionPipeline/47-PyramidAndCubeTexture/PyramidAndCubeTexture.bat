cl.exe /c /EHsc PyramidAndCubeTexture.cpp

rc.exe MyIcon.rc 

link.exe PyramidAndCubeTexture.obj MyIcon.res user32.lib gdi32.lib /SUBSYSTEM:WINDOWS
PyramidAndCubeTexture