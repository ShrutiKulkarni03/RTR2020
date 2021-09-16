cl.exe /c /EHsc DiffuseLightOnSphere.cpp

rc.exe Resources.rc

link.exe DiffuseLightOnSphere.obj Resources.res user32.lib gdi32.lib /SUBSYSTEM:WINDOWS 

DiffuseLightOnSphere
