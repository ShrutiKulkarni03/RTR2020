cl.exe /c /EHsc ThreeLightsOnSphere.cpp

rc.exe MyIcon.rc

link.exe ThreeLightsOnSphere.obj MyIcon.res user32.lib gdi32.lib /SUBSYSTEM:WINDOWS

ThreeLightsOnSphere
