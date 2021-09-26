cl.exe /c /EHsc GeometryShader.cpp

rc.exe Resources.rc

link.exe GeometryShader.obj Resources.res user32.lib gdi32.lib /SUBSYSTEM:WINDOWS /MACHINE:X64

GeometryShader
