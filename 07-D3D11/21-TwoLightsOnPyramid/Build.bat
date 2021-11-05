cl.exe /c /EHsc TwoLightsOnPyramid.cpp

rc.exe Resources.rc

link.exe TwoLightsOnPyramid.obj Resources.res user32.lib gdi32.lib /SUBSYSTEM:WINDOWS /MACHINE:X64

TwoLightsOnPyramid
