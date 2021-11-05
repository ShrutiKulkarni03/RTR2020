cl.exe /c /EHsc PerVertexPerPixelToggle.cpp

rc.exe Resources.rc

link.exe PerVertexPerPixelToggle.obj Resources.res user32.lib gdi32.lib /SUBSYSTEM:WINDOWS 

PerVertexPerPixelToggle