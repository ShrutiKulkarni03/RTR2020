cl.exe /c /EHsc PerPixelLightOnSphereAlbedo.cpp

rc.exe Resources.rc

link.exe PerPixelLightOnSphereAlbedo.obj Resources.res user32.lib gdi32.lib /SUBSYSTEM:WINDOWS 

PerPixelLightOnSphereAlbedo