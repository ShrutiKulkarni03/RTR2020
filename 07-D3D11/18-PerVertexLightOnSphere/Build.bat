cl.exe /c /EHsc PerVertexLightOnSphere.cpp

rc.exe Resources.rc

link.exe PerVertexLightOnSphere.obj Resources.res user32.lib gdi32.lib /SUBSYSTEM:WINDOWS 

PerVertexLightOnSphere