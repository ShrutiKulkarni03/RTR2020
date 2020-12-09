cl.exe /c /EHsc TwoLightsOnPyramid.cpp

rc.exe MyIcon.rc

link.exe TwoLightsOnPyramid.obj MyIcon.res user32.lib gdi32.lib /SUBSYSTEM:WINDOWS

TwoLightsOnPyramid
