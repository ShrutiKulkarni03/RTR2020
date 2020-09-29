cl.exe /c /EHsc AnimationOfDeathlyHallows.cpp

rc.exe MyIcon.rc

link.exe AnimationOfDeathlyHallows.obj MyIcon.res user32.lib gdi32.lib /SUBSYSTEM:WINDOWS

AnimationOfDeathlyHallows