cl.exe /c /EHsc ThreeLightsOnSteadySphere.cpp

rc.exe MyIcon.rc

link.exe ThreeLightsOnSteadySphere.obj MyIcon.res user32.lib gdi32.lib /SUBSYSTEM:WINDOWS

ThreeLightsOnSteadySphere
