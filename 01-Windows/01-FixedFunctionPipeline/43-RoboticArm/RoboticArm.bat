cl.exe /c /EHsc RoboticArm.cpp

rc.exe MyIcon.rc

link.exe RoboticArm.obj MyIcon.res user32.lib gdi32.lib /SUBSYSTEM:WINDOWS

RoboticArm