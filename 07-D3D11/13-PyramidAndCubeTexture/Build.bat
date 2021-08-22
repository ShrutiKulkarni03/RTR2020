cl.exe /c /EHsc /MD Texture.cpp

rc.exe Resources.rc

link.exe Texture.obj Resources.res user32.lib gdi32.lib /SUBSYSTEM:WINDOWS /MACHINE:X64

Texture
