cl.exe /c /EHsc ThreeLightsOnSphere.cpp

rc.exe Resources.rc

link.exe ThreeLightsOnSphere.obj Resources.res user32.lib gdi32.lib /SUBSYSTEM:WINDOWS 

ThreeLightsOnSphere