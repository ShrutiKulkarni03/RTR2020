cl.exe /c /EHsc ThreeRotatingLightsOnSteadySphere.cpp

rc.exe MyIcon.rc

link.exe ThreeRotatingLightsOnSteadySphere.obj MyIcon.res user32.lib gdi32.lib /SUBSYSTEM:WINDOWS

ThreeRotatingLightsOnSteadySphere
