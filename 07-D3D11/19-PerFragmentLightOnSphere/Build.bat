cl.exe /c /EHsc PerFragmentLightOnSphere.cpp

rc.exe Resources.rc

link.exe PerFragmentLightOnSphere.obj Resources.res user32.lib gdi32.lib /SUBSYSTEM:WINDOWS 

PerFragmentLightOnSphere